// TrueGlide : fiche de contrôle Word (.docx) à remplir, générée depuis un mode opératoire Markdown
// (<CODE>_mode_operatoire.md -> <CODE>_fiche.docx). Même présentation que les fiches de contrôle XPro (forme
// seulement : A4, Calibri, titres bleus, une ligne oui / non par question, cadres « À envoyer » en bas).
// Usage : npm ci && node fiche.mjs TG-P2_mode_operatoire.md
//
// Mode opératoire :
//   # Titre                          titre de la fiche
//   Date : … Drone : … Fait par : …  ligne à remplir, sous le titre
//   texte                            introduction (**gras** permis)
//   ## Avant de commencer …          étapes numérotées « 1. », sans case
//   ## <CODE>-n. Titre               section de contrôle : chaque « - question ? » devient une ligne oui / non
//   | a | b | c |                    tableau (première ligne = en-tête) ; colonne « Constaté » ajoutée
//   ## À envoyer                     puis « ### Titre » : un cadre vide par titre, « [grand] » pour un long texte
//   ![légende](photo.jpg)            photo pleine largeur (JPEG), légende dessous ; placée après les questions de la
//                                    section (chemin relatif au mode opératoire)
import fs from "node:fs";
import path from "node:path";
import {
    AlignmentType,
    BorderStyle,
    CheckBox,
    Document,
    HeightRule,
    ImageRun,
    Packer,
    Paragraph,
    ShadingType,
    Table,
    TableCell,
    TableRow,
    TextRun,
    WidthType,
} from "docx";

const BLEU = "1F3864";
const GRIS = "999999";
const FOND = "E8EEF4";
const CM = 567; // twips par centimètre
const LARGEUR = Math.round(17 * CM); // A4 (21 cm) moins 2 × 2 cm de marges
const COL_OUI_NON = Math.round(3.35 * CM);
const COL_CONSTATE = Math.round(4.2 * CM);
const bord = { style: BorderStyle.SINGLE, size: 4, color: GRIS };
const bords = { top: bord, bottom: bord, left: bord, right: bord, insideHorizontal: bord, insideVertical: bord };

// texte avec **gras** et `commande` (Consolas)
function morceaux(texte, base = {}) {
    return texte
        .split(/(\*\*[^*]+\*\*|`[^`]+`)/)
        .filter(Boolean)
        .map((m) =>
            m.startsWith("**")
                ? new TextRun({ ...base, text: m.slice(2, -2), bold: true })
                : m.startsWith("`")
                  ? new TextRun({ ...base, text: m.slice(1, -1), font: "Consolas" })
                  : new TextRun({ ...base, text: m }),
        );
}

const titre = (t) => new Paragraph({ spacing: { after: 120 }, children: [new TextRun({ text: t, bold: true, size: 30, color: BLEU })] });
const section = (t) =>
    new Paragraph({ spacing: { before: 240, after: 100 }, keepNext: true, children: [new TextRun({ text: t, bold: true, size: 25, color: BLEU })] });
const para = (t) => new Paragraph({ spacing: { after: 100 }, children: morceaux(t) });
const cellule = (enfants, largeur, fond) =>
    new TableCell({
        width: { size: largeur, type: WidthType.DXA },
        margins: { top: 60, bottom: 60, left: 100, right: 100 },
        shading: fond ? { type: ShadingType.CLEAR, fill: fond, color: "auto" } : undefined,
        children: enfants,
    });

// case à cocher cliquable, vide au départ (symboles ☐ / ☒ donnés : sinon certains lecteurs l'affichent cochée)
const caseVide = () =>
    new CheckBox({ checked: false, checkedState: { value: "2612", font: "MS Gothic" }, uncheckedState: { value: "2610", font: "MS Gothic" } });

// une question : numéro en gras + question, puis « ☐ oui ☐ non » (cases cliquables)
function ligneQuestion(numero, question) {
    const cases = new Paragraph({
        alignment: AlignmentType.CENTER,
        children: [caseVide(), new TextRun(" oui   "), caseVide(), new TextRun(" non")],
    });
    return new TableRow({
        cantSplit: true,
        children: [
            cellule([new Paragraph({ children: [new TextRun({ text: numero + " ", bold: true }), ...morceaux(question)] })], LARGEUR - COL_OUI_NON),
            cellule([cases], COL_OUI_NON),
        ],
    });
}

function tableauQuestions(code, questions) {
    return new Table({
        width: { size: LARGEUR, type: WidthType.DXA },
        columnWidths: [LARGEUR - COL_OUI_NON, COL_OUI_NON],
        borders: bords,
        rows: questions.map((q, i) => ligneQuestion(`${code}.${i + 1}`, q)),
    });
}

// tableau du mode opératoire + colonne « Constaté (oui / non, valeur) », en-tête répété sur chaque page
function tableauEtapes(lignes) {
    const [entete, ...corps] = lignes;
    const n = entete.length;
    const premiere = Math.round(1.4 * CM);
    const reste = Math.round((LARGEUR - COL_CONSTATE - premiere) / (n - 1));
    const largeurs = [premiere, ...Array(n - 1).fill(reste), COL_CONSTATE];
    const ligne = (cellules, enTete) =>
        new TableRow({
            tableHeader: enTete,
            cantSplit: true,
            children: cellules.map((t, i) =>
                cellule([new Paragraph({ children: morceaux(t, enTete ? { bold: true } : {}) })], largeurs[i], enTete ? FOND : undefined),
            ),
        });
    return new Table({
        width: { size: LARGEUR, type: WidthType.DXA },
        columnWidths: largeurs,
        borders: bords,
        rows: [ligne([...entete, "Constaté (oui / non, valeur)"], true), ...corps.map((c) => ligne([...c, ""], false))],
    });
}

// cadre vide bordé, avec son titre en gras au-dessus
function cadre(t, grand) {
    return [
        new Paragraph({ spacing: { before: 160, after: 60 }, keepNext: true, children: [new TextRun({ text: t, bold: true })] }),
        new Table({
            width: { size: LARGEUR, type: WidthType.DXA },
            columnWidths: [LARGEUR],
            borders: bords,
            rows: [
                new TableRow({
                    height: { value: Math.round((grand ? 10.6 : 2.6) * CM), rule: HeightRule.EXACT },
                    children: [cellule([new Paragraph({ children: [new TextRun({ text: "", font: "Consolas" })] })], LARGEUR)],
                }),
            ],
        }),
    ];
}

// largeur et hauteur d'une image JPEG (marqueur SOF)
function tailleJpeg(octets) {
    let i = 2;
    while (i < octets.length) {
        const marqueur = octets[i + 1];
        const longueur = octets.readUInt16BE(i + 2);
        if (marqueur >= 0xc0 && marqueur <= 0xcf && ![0xc4, 0xc8, 0xcc].includes(marqueur)) {
            return { hauteur: octets.readUInt16BE(i + 5), largeur: octets.readUInt16BE(i + 7) };
        }
        i += 2 + longueur;
    }
    throw new Error("image JPEG illisible");
}

// photo pleine largeur (17 cm = 643 points à 96 ppp), légende en italique gris 9 pt dessous
function photo(chemin, legende) {
    const octets = fs.readFileSync(chemin);
    const { largeur, hauteur } = tailleJpeg(octets);
    const l = 643;
    return [
        new Paragraph({
            spacing: { before: 160 },
            keepNext: true,
            children: [new ImageRun({ type: "jpg", data: octets, transformation: { width: l, height: Math.round((l * hauteur) / largeur) } })],
        }),
        new Paragraph({ spacing: { after: 160 }, children: [new TextRun({ text: legende, italics: true, size: 18, color: "666666" })] }),
    ];
}

function fiche(md, dossier) {
    const enfants = [];
    let code = null;
    let questions = [];
    let tableau = [];
    let photos = [];
    const vider = () => {
        if (questions.length) enfants.push(tableauQuestions(code, questions));
        if (tableau.length) enfants.push(tableauEtapes(tableau));
        for (const [chemin, legende] of photos) enfants.push(...photo(path.join(dossier, chemin), legende));
        questions = [];
        tableau = [];
        photos = [];
    };
    // lignes de texte qui se suivent : un seul paragraphe (le mode opératoire est coupé à 120 caractères)
    const lignes = [];
    for (const brut of md.replace(/\r/g, "").split("\n")) {
        const l = brut.trim();
        const precedente = lignes[lignes.length - 1];
        const texte = (x) => x && !/^(#|- |\||!\[|\d+\. )/.test(x);
        if (texte(l) && texte(precedente) && !/^Date :/.test(precedente) && !/^Date :/.test(l)) {
            lignes[lignes.length - 1] = `${precedente} ${l}`;
        } else {
            lignes.push(l);
        }
    }
    for (const l of lignes) {
        let m;
        if (!l) continue;
        if ((m = /^!\[(.*)\]\((.+)\)$/.exec(l))) {
            photos.push([m[2], m[1]]);
        } else if ((m = /^# (.+)$/.exec(l))) {
            enfants.push(titre(m[1]));
        } else if ((m = /^## (.+)$/.exec(l))) {
            vider();
            code = (/^([A-Z0-9]+(?:-[A-Z0-9]+)*-\d+)\. /.exec(m[1]) || [])[1] || null;
            enfants.push(section(m[1]));
        } else if ((m = /^### (.+?)( \[grand\])?$/.exec(l))) {
            vider();
            enfants.push(...cadre(m[1], Boolean(m[2])));
        } else if ((m = /^- (.+)$/.exec(l)) && code) {
            questions.push(m[1]);
        } else if ((m = /^- (.+)$/.exec(l))) {
            // sous-point d'une étape (« Avant de commencer ») : puce, en retrait
            enfants.push(new Paragraph({ spacing: { after: 60 }, indent: { left: 720, hanging: 240 }, children: morceaux(`•\t${m[1]}`) }));
        } else if (l.startsWith("|")) {
            tableau.push(l.replace(/^\||\|$/g, "").split("|").map((c) => c.trim()));
        } else if ((m = /^(\d+)\. (.+)$/.exec(l))) {
            enfants.push(new Paragraph({ spacing: { after: 60 }, indent: { left: 360, hanging: 360 }, children: morceaux(`${m[1]}.\t${m[2]}`) }));
        } else {
            enfants.push(para(l));
        }
    }
    vider();
    return new Document({
        creator: "Jschmiedd",
        styles: { default: { document: { run: { font: "Calibri", size: 21 } } } },
        sections: [{ properties: { page: { size: { width: 11906, height: 16838 }, margin: { top: 2 * CM, bottom: 2 * CM, left: 2 * CM, right: 2 * CM } } }, children: enfants }],
    });
}

const source = process.argv[2];
if (!source) {
    throw new Error("indiquer le mode opératoire : node fiche.mjs <CODE>_mode_operatoire.md");
}
const sortie = source.replace(/_mode_operatoire\.md$/, "_fiche.docx");
fs.writeFileSync(sortie, await Packer.toBuffer(fiche(fs.readFileSync(source, "utf8"), path.dirname(source))));
console.log(`${path.basename(source)} -> ${path.basename(sortie)}`);
