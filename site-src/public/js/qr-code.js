
/* ─────────────────────────────────────────────────────── */
/* DATA — simulated from previous pages                    */
/* In a real app these come from sessionStorage/URL params */
/* ─────────────────────────────────────────────────────── */
const reservation = {
    place:    'A-01',
    zone:     'ZONE A — NIVEAU 0',
    type:     'Standard · Couvert · PMR',
    ref:      'DCG-2026-' + Math.floor(1000 + Math.random() * 9000),
    holder:   'Jean DUPONT',
    email:    'jean.dupont@exemple.fr',
    card:     '•••• •••• •••• 4242',
    start:    getFirstNextMonth(),
    next:     getFirstMonthAfter(2),
};

function getFirstNextMonth() {
    const d = new Date();
    return new Date(d.getFullYear(), d.getMonth() + 1, 1)
        .toLocaleDateString('fr-FR', { day:'2-digit', month:'long', year:'numeric' });
}
function getFirstMonthAfter(n) {
    const d = new Date();
    return new Date(d.getFullYear(), d.getMonth() + n, 1)
        .toLocaleDateString('fr-FR', { day:'2-digit', month:'long', year:'numeric' });
}

/* ─── POPULATE ───────────────────────────────────────── */
function populate() {
    document.getElementById('rc-place').textContent  = reservation.place;
    document.getElementById('rc-zone').textContent   = reservation.zone;
    document.getElementById('rc-type').textContent   = reservation.type;
    document.getElementById('rc-ref').textContent    = reservation.ref;
    document.getElementById('rc-holder').textContent = reservation.holder;
    document.getElementById('rc-email').textContent  = reservation.email;
    document.getElementById('rc-start').textContent  = reservation.start;
    document.getElementById('rc-next').textContent   = reservation.next;
    document.getElementById('rc-card').textContent   = reservation.card;

    document.getElementById('tc-place').textContent  = reservation.place;
    document.getElementById('tc-email').textContent  = reservation.email;

    document.getElementById('qr-ref-label').textContent = reservation.ref;
}

/* ─── QR CODE ────────────────────────────────────────── */
function buildQR() {
    const qrData = JSON.stringify({
        ref:    reservation.ref,
        place:  reservation.place,
        zone:   reservation.zone,
        holder: reservation.holder,
        start:  reservation.start,
        garage: 'Davinci Custom Garage — 12 Rue de la Mécanique, Paris'
    });

    new QRCode(document.getElementById('qrcode'), {
        text:           qrData,
        width:          200,
        height:         200,
        colorDark:      '#050505',
        colorLight:     '#ffffff',
        correctLevel:   QRCode.CorrectLevel.H,
    });
}

/* ─── DOWNLOAD QR ────────────────────────────────────── */
function downloadQR() {
    const canvas = document.querySelector('#qrcode canvas');
    const img    = document.querySelector('#qrcode img');

    // ─── Cas 1 : canvas disponible (Chrome, Firefox)
    if (canvas) {
        exportCanvas(canvas);
        return;
    }

    // ─── Cas 2 : seulement une <img> (Safari, certains navigateurs)
    if (img) {
        const tempCanvas = document.createElement('canvas');
        tempCanvas.width  = 200;
        tempCanvas.height = 200;
        const ctx = tempCanvas.getContext('2d');
        ctx.drawImage(img, 0, 0, 200, 200);
        exportCanvas(tempCanvas);
        return;
    }

    console.warn('QR Code introuvable');
}

function exportCanvas(canvas) {
    const pad    = 28;
    const labelH = 42;
    const out    = document.createElement('canvas');
    out.width    = canvas.width  + pad * 2;
    out.height   = canvas.height + pad * 2 + labelH;
    const ctx    = out.getContext('2d');

    ctx.fillStyle = '#ffffff';
    ctx.fillRect(0, 0, out.width, out.height);
    ctx.drawImage(canvas, pad, pad);

    ctx.fillStyle = '#050505';
    ctx.font      = 'bold 14px monospace';
    ctx.textAlign = 'center';
    ctx.fillText(reservation.ref, out.width / 2, canvas.height + pad + 24);

    ctx.font      = '11px monospace';
    ctx.fillStyle = '#666';
    ctx.fillText('Davinci Custom Garage · Place ' + reservation.place, out.width / 2, canvas.height + pad + 40);

    const a    = document.createElement('a');
    a.download = 'QR_' + reservation.ref + '.png';
    a.href     = out.toDataURL('image/png');
    a.click();
}

/* ─── INIT ───────────────────────────────────────────── */
populate();
buildQR();
