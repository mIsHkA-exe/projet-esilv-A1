
/* ─── CARD FLIP ──────────────────────────────────────── */
function flipCard(toBack) {
    document.getElementById('card3d').classList.toggle('flipped', toBack);
}

/* ─── CARD NUMBER ────────────────────────────────────── */
function handleCardNumber(input) {
    let val = input.value.replace(/\D/g, '').slice(0, 16);
    let formatted = val.replace(/(.{4})/g, '$1  ').trim();
    input.value = formatted;

    // Display on card
    let display = val.padEnd(16, '·');
    let parts = [display.slice(0,4), display.slice(4,8), display.slice(8,12), display.slice(12,16)];
    document.getElementById('cardNumDisplay').textContent = parts.join('  ');

    // Show card type
    const badge = document.getElementById('cardTypeBadge');
    badge.classList.toggle('show', val.length >= 1);
}

/* ─── CARD NAME ──────────────────────────────────────── */
function handleName(input) {
    const val = input.value.toUpperCase();
    input.value = val;
    document.getElementById('cardNameDisplay').textContent = val || 'VOTRE NOM';
    document.getElementById('s-name').textContent = val || '—';
}

/* ─── EXPIRY ─────────────────────────────────────────── */
function handleExpiry(input) {
    let val = input.value.replace(/\D/g, '').slice(0, 4);
    if (val.length >= 3) val = val.slice(0,2) + '/' + val.slice(2);
    input.value = val;
    document.getElementById('cardExpiryDisplay').textContent = val || 'MM/AA';

    const valid = document.getElementById('expiryValid');
    valid.classList.toggle('show', val.length === 5);
}

/* ─── CVV ────────────────────────────────────────────── */
function handleCvv(input) {
    const val = input.value.replace(/\D/g, '').slice(0, 4);
    input.value = val;
    document.getElementById('cardCvvDisplay').textContent = val ? '•'.repeat(val.length) : '•••';
}

/* ─── PAYMENT SIMULATION ─────────────────────────────── */
function processPayment() {
    const numRaw = document.getElementById('cardNumber').value.replace(/\s/g,'');
    const name   = document.getElementById('cardName').value.trim();
    const expiry = document.getElementById('cardExpiry').value;
    const cvv    = document.getElementById('cardCvv').value;

    // Grab holder name fields
    const firstNameEl = document.querySelector('input[autocomplete="given-name"]');
    const lastNameEl  = document.querySelector('input[autocomplete="family-name"]');
    const emailEl     = document.querySelector('input[autocomplete="email"]');

    const firstName = firstNameEl ? firstNameEl.value.trim() : '';
    const lastName  = lastNameEl  ? lastNameEl.value.trim()  : '';
    const email     = emailEl     ? emailEl.value.trim()     : '';

    if (numRaw.length < 16)   { shake('cardNumber'); return; }
    if (name.length < 3)       { shake('cardName');   return; }
    if (expiry.length < 5)     { shake('cardExpiry'); return; }
    if (cvv.length < 3)        { shake('cardCvv');    return; }
    if (!email.includes('@'))  {
        const el = document.querySelector('input[autocomplete="email"]');
        el.style.borderColor = 'var(--red)';
        el.style.boxShadow   = '0 0 20px rgba(221,21,21,0.4)';
        setTimeout(() => { el.style.borderColor = ''; el.style.boxShadow = ''; }, 800);
        return;
    }

    const loader = document.getElementById('loading-screen');
    loader.classList.add('show');

    setTimeout(() => {
        loader.classList.remove('show');

        // ── Populate recap ────────────────────────────────
        const ref = 'DCG-2026-' + Math.floor(1000 + Math.random()*9000);
        document.getElementById('success-ref').textContent = 'REF : ' + ref;

        // Full holder name: prefer first+last fields, fallback to card name
        const holderDisplay = (firstName || lastName)
            ? [firstName, lastName.toUpperCase()].filter(Boolean).join(' ')
            : name;
        document.getElementById('s-holder').textContent = holderDisplay || '—';

        // Email
        document.getElementById('s-email').textContent        = email || '—';
        document.getElementById('s-email-inline').textContent = email || '—';

        // Card last 4
        const last4 = numRaw.slice(-4);
        document.getElementById('s-card').textContent = '•••• •••• •••• ' + last4;

        // Start date: 1st of next month
        const now   = new Date();
        const start = new Date(now.getFullYear(), now.getMonth() + 1, 1);
        const opts  = { day:'2-digit', month:'long', year:'numeric' };
        document.getElementById('s-date').textContent = start.toLocaleDateString('fr-FR', opts);

        document.getElementById('success-screen').classList.add('show');
    }, 2800);
}

function shake(id) {
    const el = document.getElementById(id);
    el.style.animation = 'none';
    el.offsetHeight; // reflow
    el.style.animation = 'shakeInput 0.4s ease';
    el.style.borderColor = 'var(--red)';
    el.style.boxShadow   = '0 0 20px rgba(221,21,21,0.4)';
    setTimeout(() => { el.style.borderColor = ''; el.style.boxShadow = ''; }, 800);
}
