
/* ─── SPOTS DATA ─────────────────────────────────────── */
const spots = [
  /* Zone A — Niveau 0 — 4 places, tous les cas de figure */
  { id:'A-01', zone:'A', location:'Niveau 0', status:'available', type:'Standard',   features:['Couvert','PMR'],       price:'89€' },
  { id:'A-02', zone:'A', location:'Niveau 0', status:'available', type:'Standard',   features:['Couvert','Borne EV'],  price:'89€' },
  { id:'A-03', zone:'A', location:'Niveau 0', status:'reserved',  type:'Standard',   features:['Couvert'],             price:'79€' },
  { id:'A-04', zone:'A', location:'Niveau 0', status:'occupied',  type:'Large',      features:['Couvert','Large'],     price:'99€' },

  /* Zone B — toutes indisponibles */
  { id:'B-01', zone:'B', location:'Niveau -1', status:'disabled', type:'Standard', features:['Couvert'],  price:'—' },
  { id:'B-02', zone:'B', location:'Niveau -1', status:'disabled', type:'Standard', features:['Couvert'],  price:'—' },
  { id:'B-03', zone:'B', location:'Niveau -1', status:'disabled', type:'Standard', features:['Couvert'],  price:'—' },
  { id:'B-04', zone:'B', location:'Niveau -1', status:'disabled', type:'Large',    features:['Couvert'],  price:'—' },
  { id:'B-05', zone:'B', location:'Niveau -1', status:'disabled', type:'Standard', features:['Couvert'],  price:'—' },
  { id:'B-06', zone:'B', location:'Niveau -1', status:'disabled', type:'Standard', features:['Couvert'],  price:'—' },

  /* Zone C — toutes indisponibles */
  { id:'C-01', zone:'C', location:'Extérieur', status:'disabled', type:'Standard', features:['Extérieur'], price:'—' },
  { id:'C-02', zone:'C', location:'Extérieur', status:'disabled', type:'Standard', features:['Extérieur'], price:'—' },
  { id:'C-03', zone:'C', location:'Extérieur', status:'disabled', type:'Standard', features:['Extérieur'], price:'—' },
  { id:'C-04', zone:'C', location:'Extérieur', status:'disabled', type:'Standard', features:['Extérieur'], price:'—' },
  { id:'C-05', zone:'C', location:'Extérieur', status:'disabled', type:'Large',    features:['Extérieur'], price:'—' },
  { id:'C-06', zone:'C', location:'Extérieur', status:'disabled', type:'Standard', features:['Extérieur'], price:'—' },
];

const statusLabel = {
    available: 'Disponible',
    reserved:  'Réservée',
    occupied:  'Occupée',
    disabled:  'Indisponible'
};

function buildCard(spot, delay = 0) {
    const card = document.createElement('div');
    const isDisabled = spot.status === 'disabled';
    card.className = `spot-card ${isDisabled ? 'disabled-spot' : spot.status}`;
    card.dataset.status = spot.status;
    card.dataset.zone   = spot.zone;
    card.style.animationDelay = `${delay}ms`;

    const features = spot.features.map(f => `<span class="feature-tag">${f}</span>`).join('');

    const actionBtn = spot.status === 'available'
        ? `<button class="btn-buy" onclick="openModal('${spot.id}')">Acheter</button>`
        : `<span class="btn-blocked">${isDisabled ? '—' : 'Indisponible'}</span>`;

    const badgeClass = isDisabled ? 'disabled' : spot.status;

    card.innerHTML = `
        <span class="status-badge ${badgeClass}" title="${statusLabel[spot.status]}"></span>
        <div class="spot-number">${spot.id}</div>
        <div class="spot-location"><span>📍</span><span>${spot.location}</span></div>
        <div class="spot-features">${features}</div>
        <div class="spot-action">
            <span class="status-text">${statusLabel[spot.status]}</span>
            ${actionBtn}
        </div>
    `;
    return card;
}

function render() {
    ['A','B','C'].forEach(zone => {
        const container = document.getElementById(`zone-${zone}`);
        const zoneSpots = spots.filter(s => s.zone === zone);
        container.innerHTML = '';
        zoneSpots.forEach((s, i) => container.appendChild(buildCard(s, i * 55)));
    });
}
render();

/* ─── FILTER ─────────────────────────────────────────── */
function filterSpots(filter, btn) {
    document.querySelectorAll('.filter-btn').forEach(b => b.classList.remove('active'));
    btn.classList.add('active');

    let anyVisible = false;

    ['A','B','C'].forEach(zone => {
        const container = document.getElementById(`zone-${zone}`);
        const section   = container.closest('.zone-section');
        const cards     = container.querySelectorAll('.spot-card');
        let zoneVisible = 0;

        cards.forEach(card => {
            const st = card.dataset.status;
            const show = filter === 'all'
                || st === filter
                || (filter !== 'disabled' && st === 'disabled' && false) // never hide disabled in other filters
                || (filter === 'all');

            // Simple rule: show all for 'all', else match status
            const shouldShow = filter === 'all' || card.dataset.status === filter;
            card.style.display = shouldShow ? '' : 'none';
            if (shouldShow) { zoneVisible++; anyVisible = true; }
        });

        section.style.display = zoneVisible === 0 ? 'none' : '';
        document.getElementById(`count-${zone}`).textContent =
            `${zoneVisible} PLACE${zoneVisible !== 1 ? 'S' : ''}`;
    });

    document.getElementById('empty-state').style.display = anyVisible ? 'none' : 'block';
}

/* ─── MODAL ──────────────────────────────────────────── */
let selectedSpot = null;

function openModal(id) {
    selectedSpot = spots.find(s => s.id === id);
    if (!selectedSpot || selectedSpot.status !== 'available') return;

    document.getElementById('modal-title').textContent = `Place ${selectedSpot.id}`;
    document.getElementById('modal-sub').textContent   = `Zone ${selectedSpot.zone} — ${selectedSpot.location} · ${selectedSpot.features.join(', ')}`;
    document.getElementById('modal-num').textContent   = selectedSpot.id;
    document.getElementById('modal-zone').textContent  = `Zone ${selectedSpot.zone}`;
    document.getElementById('modal-loc').textContent   = selectedSpot.location;
    document.getElementById('modal-type').textContent  = selectedSpot.type;
    document.getElementById('modal-price').textContent = selectedSpot.price;

    document.getElementById('modal-overlay').classList.add('open');
    document.body.style.overflow = 'hidden';
}

function closeModal(e) {
    if (e.target === document.getElementById('modal-overlay')) closeModalDirect();
}
function closeModalDirect() {
    document.getElementById('modal-overlay').classList.remove('open');
    document.body.style.overflow = '';
}

function confirmSelection() {
    if (!selectedSpot) return;
    alert(`Place ${selectedSpot.id} sélectionnée ✓\nRedirection vers le formulaire de réservation…`);
    closeModalDirect();
}

document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModalDirect(); });
