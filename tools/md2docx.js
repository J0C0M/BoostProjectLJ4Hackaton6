// Minimal Markdown -> DOCX converter for the ontwerpdocument (headings, paragraphs, tables, lists, quotes, code).
const fs = require('fs');
const {
  Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell, WidthType, ShadingType,
  BorderStyle, HeadingLevel, LevelFormat, AlignmentType, Footer, PageNumber, ExternalHyperlink,
} = require('docx');

const [, , inPath, outPath] = process.argv;
const lines = fs.readFileSync(inPath, 'utf8').split('\n');

const FONT = 'Arial';
const MONO = 'Courier New';
const ACCENT = 'B0174B';
const CONTENT_W = 9026; // A4 minus 1" margins

function tr(text, opts) {
  return text.split('\n').map((t, k) => new TextRun({ text: t, break: k > 0 ? 1 : undefined, ...opts }));
}
function inline(text, base = {}) {
  const runs = [];
  const re = /(\*\*([^*]+?)\*\*|`([^`]+)`|\[([^\]]+)\]\(([^)]+)\))/g;
  let last = 0, m;
  while ((m = re.exec(text))) {
    if (m.index > last) runs.push(...tr(text.slice(last, m.index), { font: FONT, ...base }));
    if (m[2]) runs.push(...inline(m[2], { ...base, bold: true }));
    else if (m[3]) runs.push(new TextRun({ text: m[3], font: MONO, ...base, size: (base.size || 21) - 2 }));
    else if (m[4]) {
      if (/^https?:/.test(m[5])) {
        runs.push(new ExternalHyperlink({ link: m[5], children: [new TextRun({ text: m[4], font: FONT, style: 'Hyperlink', ...base })] }));
      } else runs.push(...inline(m[4], base));
    }
    last = re.lastIndex;
  }
  if (last < text.length) runs.push(...tr(text.slice(last), { font: FONT, ...base }));
  return runs;
}

const cellBorder = { style: BorderStyle.SINGLE, size: 4, color: 'BFBFBF' };
function table(rows) {
  const cells = rows.map(r => r.replace(/^\||\|$/g, '').split('|').map(c => c.trim()));
  const header = cells[0];
  const body = cells.slice(2);
  const n = header.length;
  const weights = header.map((_, i) => Math.max(8, Math.min(60, ...[0], Math.max(...cells.filter((_, k) => k !== 1).map(r => (r[i] || '').length)))));
  const total = weights.reduce((a, b) => a + b, 0);
  const widths = weights.map(w => Math.floor(CONTENT_W * w / total));
  widths[n - 1] += CONTENT_W - widths.reduce((a, b) => a + b, 0);
  const mk = (row, isHead) => new TableRow({
    tableHeader: isHead,
    cantSplit: true,
    children: row.map((c, i) => new TableCell({
      width: { size: widths[i], type: WidthType.DXA },
      borders: { top: cellBorder, bottom: cellBorder, left: cellBorder, right: cellBorder },
      shading: isHead ? { type: ShadingType.CLEAR, fill: 'F6D5DE', color: 'auto' } : undefined,
      margins: { top: 60, bottom: 60, left: 100, right: 100 },
      children: [new Paragraph({ children: inline(c, { size: 18, bold: isHead || undefined }) })],
    })),
  });
  return new Table({
    width: { size: CONTENT_W, type: WidthType.DXA },
    columnWidths: widths,
    rows: [mk(header, true), ...body.map(r => mk(r, false))],
  });
}

const children = [];
let i = 0;
let listInstance = 0;
const spacer = () => children.push(new Paragraph({ spacing: { after: 60 }, children: [] }));
while (i < lines.length) {
  const line = lines[i];
  if (!line.trim() || line.trim() === '---') { i++; continue; }
  if (line.startsWith('```')) {
    i++;
    while (i < lines.length && !lines[i].startsWith('```')) {
      children.push(new Paragraph({
        spacing: { after: 0, line: 240 },
        shading: { type: ShadingType.CLEAR, fill: 'F4F4F4', color: 'auto' },
        children: [new TextRun({ text: lines[i] || ' ', font: MONO, size: 15 })],
      }));
      i++;
    }
    i++; spacer(); continue;
  }
  if (line.startsWith('|')) {
    const rows = [];
    while (i < lines.length && lines[i].startsWith('|')) rows.push(lines[i++]);
    children.push(table(rows)); spacer(); continue;
  }
  let m;
  if ((m = line.match(/^(#{1,3}) (.*)/))) {
    const lvl = m[1].length;
    const heading = lvl === 1 ? HeadingLevel.TITLE : lvl === 2 ? HeadingLevel.HEADING_1 : HeadingLevel.HEADING_2;
    children.push(new Paragraph({ heading, children: inline(m[2]) }));
    i++; continue;
  }
  if (line.startsWith('>')) {
    const parts = [];
    while (i < lines.length && lines[i].startsWith('>')) parts.push(lines[i++].replace(/^>\s?/, ''));
    children.push(new Paragraph({
      indent: { left: 360 },
      border: { left: { style: BorderStyle.SINGLE, size: 12, color: ACCENT, space: 8 } },
      children: inline(parts.join(' '), { italics: true, color: '555555' }),
    }));
    continue;
  }
  if ((m = line.match(/^- \[( |x)\] (.*)/))) {
    children.push(new Paragraph({ indent: { left: 360, hanging: 360 }, spacing: { after: 60 },
      children: [new TextRun({ text: (m[1] === 'x' ? '☑' : '☐') + '\t', font: 'Segoe UI Symbol' }), ...inline(m[2])] }));
    i++; continue;
  }
  if ((m = line.match(/^- (.*)/))) {
    children.push(new Paragraph({ numbering: { reference: 'bullets', level: 0 }, spacing: { after: 60 }, children: inline(m[1]) }));
    i++; continue;
  }
  if ((m = line.match(/^\d+\. (.*)/))) {
    if (!/^\d+\. /.test(lines[i - 1] || '')) listInstance++;
    children.push(new Paragraph({ numbering: { reference: 'numbers', level: 0, instance: listInstance }, spacing: { after: 60 }, children: inline(m[1]) }));
    i++; continue;
  }
  // paragraph: gather consecutive plain lines; trailing double-space = line break
  const raw = [];
  while (i < lines.length && lines[i].trim() && !/^(#|\||>|```|- |\d+\. |---)/.test(lines[i])) raw.push(lines[i++]);
  const text = raw.map((l, k) => (k === 0 ? '' : raw[k - 1].endsWith('  ') ? '\n' : ' ') + l.trim()).join('');
  const runs = inline(text);
  children.push(new Paragraph({ children: runs }));
}

const doc = new Document({
  creator: 'Projectteam Hackaton 6',
  title: 'Ontwerpdocument – Niet-zo-single-feestje',
  styles: {
    default: { document: { run: { font: FONT, size: 21 }, paragraph: { spacing: { after: 120, line: 276 } } } },
    paragraphStyles: [
      { id: 'Title', name: 'Title', basedOn: 'Normal', next: 'Normal', run: { font: FONT, size: 44, bold: true, color: ACCENT },
        paragraph: { spacing: { before: 0, after: 240 }, keepNext: true } },
      { id: 'Heading1', name: 'Heading 1', basedOn: 'Normal', next: 'Normal', quickFormat: true,
        run: { font: FONT, size: 30, bold: true, color: ACCENT },
        paragraph: { spacing: { before: 360, after: 120 }, keepNext: true, outlineLevel: 0 } },
      { id: 'Heading2', name: 'Heading 2', basedOn: 'Normal', next: 'Normal', quickFormat: true,
        run: { font: FONT, size: 24, bold: true, color: '333333' },
        paragraph: { spacing: { before: 240, after: 80 }, keepNext: true, outlineLevel: 1 } },
    ],
  },
  numbering: { config: [
    { reference: 'bullets', levels: [{ level: 0, format: LevelFormat.BULLET, text: '•', alignment: AlignmentType.LEFT,
      style: { paragraph: { indent: { left: 360, hanging: 360 } } } }] },
    { reference: 'numbers', levels: [{ level: 0, format: LevelFormat.DECIMAL, text: '%1.', alignment: AlignmentType.LEFT,
      style: { paragraph: { indent: { left: 360, hanging: 360 } } } }] },
  ] },
  sections: [{
    properties: { page: { margin: { top: 1440, bottom: 1440, left: 1440, right: 1440 } } },
    footers: { default: new Footer({ children: [new Paragraph({ alignment: AlignmentType.RIGHT,
      children: [new TextRun({ children: ['Ontwerpdocument Smèrrig-hart  ·  pagina ', PageNumber.CURRENT], font: FONT, size: 16, color: '888888' })] })] }) },
    children,
  }],
});

Packer.toBuffer(doc).then(b => { fs.writeFileSync(outPath, b); console.log('written', outPath); });
