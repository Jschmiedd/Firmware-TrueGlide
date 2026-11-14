const fs = require("fs");
const { Document, Packer, Paragraph, TextRun, Table, TableRow, TableCell, WidthType, ShadingType, BorderStyle, HeadingLevel, AlignmentType } = require("docx");

const LARG = 9638; // A4 - 2 cm x 2
const bord = { style: BorderStyle.SINGLE, size: 4, color: "B0B7C3" };
const borders = { top: bord, bottom: bord, left: bord, right: bord };

function cell(text, w, head) {
  return new TableCell({
    width: { size: w, type: WidthType.DXA },
    borders,
    shading: head ? { type: ShadingType.CLEAR, color: "auto", fill: "1F3864" } : undefined,
    margins: { top: 60, bottom: 60, left: 100, right: 100 },
    children: [new Paragraph({ children: [new TextRun({ text, bold: !!head, color: head ? "FFFFFF" : undefined, size: 19 })] })],
  });
}
function table(cols, rows) {
  const total = cols.reduce((a, c) => a + c[1], 0);
  const widths = cols.map((c) => Math.round((c[1] / total) * LARG));
  widths[widths.length - 1] += LARG - widths.reduce((a, b) => a + b, 0);
  return new Table({
    width: { size: LARG, type: WidthType.DXA },
    columnWidths: widths,
    rows: [
      new TableRow({ tableHeader: true, children: cols.map((c, i) => cell(c[0], widths[i], true)) }),
      ...rows.map((r) => new TableRow({ children: r.map((t, i) => cell(t, widths[i], false)) })),
    ],
  });
}
const h1 = (t) => new Paragraph({ heading: HeadingLevel.HEADING_1, children: [new TextRun(t)] });
const p = (t) => new Paragraph({ spacing: { after: 120 }, children: [new TextRun({ text: t, size: 20 })] });
const vide = () => new Paragraph({ children: [] });

const doc = new Document({
  creator: "TrueGlide",
  styles: {
    default: { document: { run: { font: "Calibri", size: 20 } } },
    paragraphStyles: [
      { id: "Title", name: "Title", basedOn: "Normal", run: { font: "Calibri", size: 36, bold: true, color: "1F3864" }, paragraph: { spacing: { before: 0, after: 120 }, keepNext: true } },
      { id: "Heading1", name: "Heading 1", basedOn: "Normal", next: "Normal", quickFormat: true, run: { font: "Calibri", size: 26, bold: true, color: "1F3864" }, paragraph: { spacing: { before: 280, after: 120 }, keepNext: true, outlineLevel: 0 } },
    ],
  },
  sections: [{
    properties: { page: { size: { width: 11906, height: 16838 }, margin: { top: 1134, bottom: 1134, left: 1134, right: 1134 } } },
    children: [
      new Paragraph({ heading: HeadingLevel.TITLE, children: [new TextRun("Contributions externes : règles, recommandations et contrôles")] }),
      p("Johann (TRUEGLIDE), 14/11/2026. Résumé de la marche à suivre (trueglide/controle/contributions/MARCHE_A_SUIVRE.md) et des leçons de la #590, de la #592 et de la #593. Chaque règle a sa source."),

      h1("1. Recommandations de Chris (stylesuxx/esc-configurator)"),
      table([["Règle", 3], ["Source", 2], ["Comment l'appliquer", 4]], [
        ["Pas de commentaire quand le code le montre déjà", "#590 (13/11) : « Implicit. Please remove this comment. » ; #592 (14/11)", "Pour chaque commentaire ajouté : « le code le dit-il déjà ? ». Si oui : le retirer. Garder seulement un pourquoi invisible"],
        ["Messages courts, dans le style des messages voisins", "#592 (14/11) : « Gave up flashing ESC {{index}} - no progress for {{seconds}} s, check the ESC power »", "Comparer à « Failed flashing ESC {{index}} - check file type » : l'action d'abord, un tiret, un conseil"],
        ["Base : branche develop", "README, « Contributing »", "Toujours partir de develop à jour"],
        ["Fournir des tests quand c'est possible", "README", "Un test qui échoue sans la correction et passe avec"],
        ["Respecter le lint", "README", "0 erreur, aucun avertissement nouveau (comparer avant et après)"],
        ["Prévenir Chris par mail", "Règle de Johann", "Mail court ; ou bouton « Re-request review » ; jamais « Resolve » soi-même"],
      ]),

      h1("2. Règles de Betaflight (firmware et App)"),
      table([["Règle", 3], ["Source", 2], ["Comment l'appliquer", 4]], [
        ["L'auteur doit comprendre et expliquer chaque ligne", "AI_POLICY.md, AGENTS.md", "Fiche d'explication en français, lue par Johann avant l'envoi"],
        ["Base : branche master", "Modèle de PR, point 2", "—"],
        ["Une seule fonction par PR", "Modèle de PR, point 4 ; AGENTS.md", "Découper si plusieurs sujets"],
        ["Un seul commit, de préférence", "Modèle de PR, point 5", "Squash avant l'envoi"],
        ["Tests et contrôles", "AGENTS.md ; modèle de PR", "Firmware : make test, make EXTRA_FLAGS=-Werror checks, build de la carte. App : yarn lint, vitest run"],
        ["Anglais technique simplifié", "AGENTS.md (ASD-STE100)", "Phrases courtes, voix active"],
        ["Si la modification peut avoir un effet en vol : journal Blackbox d'un vrai vol, sinon pas de fusion. SITL, banc et tests unitaires ne remplacent pas un vol", "AGENTS.md firmware, § Blackbox logs and flight tests (l.123-125, l.135)", "PR en brouillon jusqu'au journal (ex. #16023)"],
        ["Chaque DEBUG_SET() a une annotation //!< (libellé, unité)", "AGENTS.md firmware, l.102", "Le firmware de TEST cdd6f38 n'en a pas : il ne va jamais tel quel dans une PR"],
        ["2 PR ouvertes au plus (App, nouveau contributeur)", "blckmn, #5791, 14/11", "Firmware : rester à 2 par prudence"],
        ["L'IA est autorisée, sans obligation de la déclarer", "AI_POLICY.md", "L'auteur reste responsable de tout"],
      ]),

      h1("3. Erreurs commises et parades"),
      table([["Erreur", 3], ["Cause", 3], ["Parade (règle et contrôle)", 4]], [
        ["Commentaires inutiles retrouvés par Chris dans la #592, après la #590", "Remarque appliquée à une seule ligne, pas à toutes les PR", "Phase G : toute remarque est appliquée à toutes les PR ouvertes et aux branches prêtes. Tableau « remarques reçues »"],
        ["Code écrit et contrôlé par la même discussion", "Pas de regard extérieur", "Une discussion écrit, l'autre contrôle (phase D)"],
        ["Commits au nom de « Claude » (#592, #593)", "Identité par défaut de la discussion", "Auteur = Jschmiedd + ligne « Co-Authored-By: Claude ». verif_pr.sh bloque"],
        ["Envoi avant le contrôle, malentendu pendant le repas", "Deux discussions demandaient le OUI en même temps", "Une seule discussion demande le OUI : celle qui pousse (F.0)"],
        ["Identifiant de la carte dans des fichiers et dans le journal Blackbox", "Sauvegardes complètes ; en-tête DeviceUID", "diff all bare ; journal masqué ; verif_pr.sh bloque mcu_id, signature, DeviceUID"],
        ["Mauvais réglage OSD (MAX7456 au lieu de MSP)", "Matériel supposé, pas vérifié", "Relire diff all et status réels avant tout réglage matériel"],
        ["13/11 (discussion TRUEGLIDE) : écrit que #15533 ne parlait pas de la SpeedyBee F405 AIO, alors qu'elle y est citée dans les commentaires et dans #15538", "Seul le 1er message lu", "Lire toute la page (commentaires, PR liées) avant de citer"],
        ["Cause supposée écrite comme un fait (baromètre)", "Hypothèse non prouvée", "Seulement des constats ; sinon « non vérifié »"],
      ]),

      h1("4. La marche à suivre, en bref"),
      table([["Phase", 2], ["Qui", 2], ["Quoi", 6]], [
        ["A. Avant d'écrire", "Rédacteur", "Lire les règles du projet ; chercher les doublons ; un seul sujet ; écrire le test qui échoue"],
        ["B. Écrire", "Rédacteur", "Style du code voisin ; aucun commentaire évident ; messages comme les voisins ; aucune donnée privée"],
        ["C. Contrôles auto", "Rédacteur", "Tests, lint (avant/après), verif_pr.sh sans point bloquant"],
        ["D. Contrôle indépendant", "Autre discussion", "Relire ligne par ligne, relancer les tests, OUI ou NON avec corrections"],
        ["E. Fiche d'explication", "Rédacteur, puis Johann", "Johann doit pouvoir expliquer sans outil"],
        ["E bis. Lecture VS Code", "Johann, guidé", "Lire le diff ; Copilot seulement pour chercher, jamais pour écrire, jamais sur des données privées"],
        ["F. Envoi", "Discussion qui pousse", "OUI écrit de Johann ; un seul envoi groupé ; vérifier le commit et les contrôles sur GitHub"],
        ["G. Après une remarque", "Les deux", "Appliquer partout ; refaire C, D, F ; ne jamais cliquer « Resolve »"],
      ]),

      h1("5. Ce que bloque le script verif_pr.sh"),
      table([["Contrôle", 4], ["Effet", 2]], [
        ["mcu_id, DeviceUID, ligne « signature », hexadécimal de 24 caractères", "BLOQUANT"],
        ["XPRO, chemins locaux (/home/, /tmp/, scratchpad)", "BLOQUANT"],
        ["Espaces en fin de ligne", "BLOQUANT"],
        ["Auteur d'un commit autre que Jschmiedd (sauf 4 anciens commits laissés)", "BLOQUANT"],
        ["Ligne « Co-Authored-By: Claude » absente", "ATTENTION"],
        ["Liste des commentaires et des messages ajoutés", "À juger par l'humain"],
        ["Plus d'un commit (Betaflight)", "ATTENTION"],
      ]),
      vide(),
      p("Projets prévus, mêmes règles, à sourcer au démarrage : INAV, ArduPilot, Mission Planner, ExpressLRS, EdgeTX, Bluejay, AM32, Blackbox Explorer."),
    ],
  }],
});
Packer.toBuffer(doc).then((b) => fs.writeFileSync(process.argv[2], b));
