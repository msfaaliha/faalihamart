/**
 * FaalihaMart Web Application Engine
 * Native C++20 Marketplace Frontend with Dual-Mode (Live REST API & Local Mock Fallback)
 */

// Initial Seed Data matching db/seed.sql
const DEFAULT_SEED_DATA = {
  users: [
    { id: 1, name: "Admin Faaliha", email: "admin@faalihamart.com", role: "ADMIN" },
    { id: 2, name: "Apex Tech Gear", email: "techseller@faalihamart.com", role: "SELLER" },
    { id: 3, name: "Urban Trendz", email: "styleseller@faalihamart.com", role: "SELLER" },
    { id: 4, name: "Alice Smith", email: "alice@faalihamart.com", role: "BUYER" },
    { id: 5, name: "Bob Jones", email: "bob@faalihamart.com", role: "BUYER" }
  ],
  products: [
    {
      id: 1, seller_id: 2, seller_name: "Apex Tech Gear",
      name: "Noise-Cancelling Wireless Headphones",
      description: "Premium over-ear acoustic headphones with 40-hour battery life and spatial audio.",
      price_cents: 14999, price_dollars: 149.99, price_formatted: "₹149.99",
      stock_qty: 25, category: "Electronics",
      image_url: "https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=500&q=80"
    },
    {
      id: 2, seller_id: 2, seller_name: "Apex Tech Gear",
      name: "Mechanical Gaming Keyboard RGB",
      description: "Custom mechanical switches, per-key RGB backlighting, and detachable braided USB-C cable.",
      price_cents: 8999, price_dollars: 89.99, price_formatted: "₹89.99",
      stock_qty: 14, category: "Electronics",
      image_url: "https://images.unsplash.com/photo-1587829741301-dc798b83add3?w=500&q=80"
    },
    {
      id: 3, seller_id: 2, seller_name: "Apex Tech Gear",
      name: "Ultra-Slim 4K USB-C Monitor 27\"",
      description: "Crisp IPS panel, 99% sRGB color gamut, HDR400, and 65W power delivery.",
      price_cents: 29999, price_dollars: 299.99, price_formatted: "₹299.99",
      stock_qty: 8, category: "Electronics",
      image_url: "https://images.unsplash.com/photo-1527443224154-c4a3942d3acf?w=500&q=80"
    },
    {
      id: 4, seller_id: 3, seller_name: "Urban Trendz",
      name: "Classic Organic Cotton Hoodie",
      description: "Heavyweight brushed fleece hoodie tailored from 100% sustainable organic cotton.",
      price_cents: 4999, price_dollars: 49.99, price_formatted: "₹49.99",
      stock_qty: 40, category: "Apparel",
      image_url: "https://images.unsplash.com/photo-1556905055-8f358a7a47b2?w=500&q=80"
    },
    {
      id: 5, seller_id: 3, seller_name: "Urban Trendz",
      name: "Waterproof Commuter Backpack",
      description: "Minimalist weatherproof everyday backpack featuring a padded 16-inch laptop compartment.",
      price_cents: 7499, price_dollars: 74.99, price_formatted: "₹74.99",
      stock_qty: 18, category: "Apparel",
      image_url: "https://images.unsplash.com/photo-1553062407-98eeb64c6a62?w=500&q=80"
    },
    {
      id: 6, seller_id: 3, seller_name: "Urban Trendz",
      name: "Aromatic Soy Wax Candle Set",
      description: "Hand-poured triple-scented essential oil candles with cedarwood, vanilla, and amber notes.",
      price_cents: 2499, price_dollars: 24.99, price_formatted: "₹24.99",
      stock_qty: 50, category: "Home & Living",
      image_url: "https://images.unsplash.com/photo-1603006905003-be475563bc59?w=500&q=80"
    },
    {
      id: 7, seller_id: 2, seller_name: "Apex Tech Gear",
      name: "Ergonomic Vertical Wireless Mouse",
      description: "Designed to reduce forearm strain, featuring optical tracking and silent clicks.",
      price_cents: 3499, price_dollars: 34.99, price_formatted: "₹34.99",
      stock_qty: 30, category: "Electronics",
      image_url: "https://images.unsplash.com/photo-1615663245857-ac93bb7c39e7?w=500&q=80"
    },
    {
      id: 8, seller_id: 3, seller_name: "Urban Trendz",
      name: "Insulated Stainless Steel Tumbler 32oz",
      description: "Double-wall vacuum insulation keeps drinks cold for 24h or piping hot for 12h.",
      price_cents: 1999, price_dollars: 19.99, price_formatted: "₹19.99",
      stock_qty: 65, category: "Home & Living",
      image_url: "https://images.unsplash.com/photo-1514432324607-a09d9b4aefdd?w=500&q=80"
    }
  ],
  orders: [
    {
      id: 1, buyer_id: 4, buyer_name: "Alice Smith", buyer_email: "alice@faalihamart.com",
      status: "DELIVERED", total_amount_cents: 19998, total_amount_formatted: "₹199.98",
      created_at: "2026-09-18 10:30:00",
      items: [
        { id: 1, product_id: 1, product_name: "Noise-Cancelling Wireless Headphones", quantity: 1, unit_price_cents: 14999, unit_price_formatted: "₹149.99", seller_id: 2 },
        { id: 2, product_id: 4, product_name: "Classic Organic Cotton Hoodie", quantity: 1, unit_price_cents: 4999, unit_price_formatted: "₹49.99", seller_id: 3 }
      ]
    },
    {
      id: 2, buyer_id: 4, buyer_name: "Alice Smith", buyer_email: "alice@faalihamart.com",
      status: "SHIPPED", total_amount_cents: 8999, total_amount_formatted: "₹89.99",
      created_at: "2026-09-20 14:15:00",
      items: [
        { id: 3, product_id: 2, product_name: "Mechanical Gaming Keyboard RGB", quantity: 1, unit_price_cents: 8999, unit_price_formatted: "₹89.99", seller_id: 2 }
      ]
    },
    {
      id: 3, buyer_id: 5, buyer_name: "Bob Jones", buyer_email: "bob@faalihamart.com",
      status: "CONFIRMED", total_amount_cents: 14999, total_amount_formatted: "₹149.99",
      created_at: "2026-09-21 09:00:00",
      items: [
        { id: 4, product_id: 1, product_name: "Noise-Cancelling Wireless Headphones", quantity: 1, unit_price_cents: 14999, unit_price_formatted: "₹149.99", seller_id: 2 }
      ]
    }
  ],
  reviews: [
    { id: 1, product_id: 1, user_id: 4, user_name: "Alice Smith", rating: 5, comment: "Absolutely incredible sound quality and comfortable padding for long work sessions.", created_at: "2026-09-19" },
    { id: 2, product_id: 4, user_id: 4, user_name: "Alice Smith", rating: 4, comment: "Very soft material and true to size fit. Highly recommended!", created_at: "2026-09-19" },
    { id: 3, product_id: 2, user_id: 5, user_name: "Bob Jones", rating: 5, comment: "Keys feel tactile and responsive. Great build quality!", created_at: "2026-09-21" }
  ],
  cart: {} // keyed by user_id
};

class AppState {
  constructor() {
    this.currentUser = JSON.parse(localStorage.getItem("faaliha_user")) || DEFAULT_SEED_DATA.users[3]; // Default Alice (Buyer)
    this.activeTab = "browse";
    this.activeCategory = "ALL";
    this.searchKeyword = "";
    this.isBackendOnline = false;

    // Auto-migrate cached localStorage if updated to INR currency
    if (localStorage.getItem("faaliha_currency") !== "INR") {
      localStorage.removeItem("faaliha_store");
      localStorage.setItem("faaliha_currency", "INR");
    }

    // Load or initialize local state
    if (!localStorage.getItem("faaliha_store")) {
      localStorage.setItem("faaliha_store", JSON.stringify(DEFAULT_SEED_DATA));
    }
    this.db = JSON.parse(localStorage.getItem("faaliha_store"));
  }

  save() {
    localStorage.setItem("faaliha_store", JSON.stringify(this.db));
    localStorage.setItem("faaliha_user", JSON.stringify(this.currentUser));
  }
}

const state = new AppState();

// UI Toast Helper
function showToast(message, type = "info") {
  const container = document.getElementById("toast-container");
  const toast = document.createElement("div");
  toast.className = `toast ${type}`;
  toast.innerHTML = `<span>${message}</span>`;
  container.appendChild(toast);
  setTimeout(() => {
    toast.style.opacity = "0";
    setTimeout(() => toast.remove(), 300);
  }, 3000);
}

// Health Check
async function checkBackendHealth() {
  const badge = document.getElementById("health-badge");
  try {
    const res = await fetch("/api/v1/health", { method: "GET" });
    if (res.ok) {
      const json = await res.json();
      state.isBackendOnline = true;
      badge.innerHTML = `<span class="health-dot"></span> C++ Drogon: ${json.status || "UP"}`;
      badge.style.background = "#dcfce7";
      badge.style.color = "#166534";
      return;
    }
  } catch (e) {
    // Expected when opening locally without compiled server running
  }
  state.isBackendOnline = false;
  badge.innerHTML = `<span class="health-dot" style="background:#0ea5e9;"></span> Preview Mode (Interactive)`;
  badge.style.background = "#e0f2fe";
  badge.style.color = "#0369a1";
}

// Navigation & Tab Switching
function switchTab(tabName) {
  state.activeTab = tabName;
  document.querySelectorAll(".tab-btn").forEach(btn => {
    btn.classList.toggle("active", btn.dataset.tab === tabName);
  });

  document.getElementById("view-browse").style.display = tabName === "browse" ? "block" : "none";
  document.getElementById("view-orders").style.display = tabName === "orders" ? "block" : "none";
  document.getElementById("view-seller").style.display = tabName === "seller" ? "block" : "none";
  document.getElementById("view-admin").style.display = tabName === "admin" ? "block" : "none";

  if (tabName === "browse") renderProducts();
  if (tabName === "orders") renderOrders();
  if (tabName === "seller") renderSellerDashboard();
  if (tabName === "admin") renderAdminPanel();
}

// User Switching
function setCurrentUser(userId) {
  const user = state.db.users.find(u => u.id === Number(userId));
  if (user) {
    state.currentUser = user;
    state.save();
    updateUserUI();
    showToast(`Switched account to ${user.name} (${user.role})`);
    switchTab(state.activeTab);
    updateCartCount();
  }
}

function updateUserUI() {
  const userSelect = document.getElementById("user-switcher");
  userSelect.value = state.currentUser ? state.currentUser.id : "";
  
  const role = state.currentUser ? state.currentUser.role : "BUYER";
  document.getElementById("tab-seller").style.display = (role === "SELLER" || role === "ADMIN") ? "inline-block" : "none";
  document.getElementById("tab-admin").style.display = (role === "ADMIN") ? "inline-block" : "none";
}

// Render Products Catalog
function renderProducts() {
  const grid = document.getElementById("product-grid");
  grid.innerHTML = "";

  const filtered = state.db.products.filter(p => {
    const matchesCat = state.activeCategory === "ALL" || p.category === state.activeCategory;
    const matchesKw = !state.searchKeyword || 
      p.name.toLowerCase().includes(state.searchKeyword.toLowerCase()) || 
      p.description.toLowerCase().includes(state.searchKeyword.toLowerCase());
    return matchesCat && matchesKw;
  });

  if (filtered.length === 0) {
    grid.innerHTML = `<div style="grid-column: 1/-1; text-align: center; padding: 3rem; color: var(--gray-500);">
      <h3>No products found matching your search.</h3>
    </div>`;
    return;
  }

  filtered.forEach(p => {
    const card = document.createElement("div");
    card.className = "product-card";
    const stockClass = p.stock_qty <= 0 ? "out" : (p.stock_qty < 5 ? "low" : "");
    const stockText = p.stock_qty <= 0 ? "Out of Stock" : `${p.stock_qty} in stock`;

    card.innerHTML = `
      <div class="card-img-wrap">
        <img src="${p.image_url}" alt="${p.name}" loading="lazy" />
        <span class="card-cat-badge">${p.category}</span>
      </div>
      <div class="card-body">
        <div style="font-size: 0.75rem; color: var(--gray-500); margin-bottom: 0.2rem;">Seller: ${p.seller_name || "Merchant"}</div>
        <h3 class="card-title">${p.name}</h3>
        <p class="card-desc">${p.description}</p>
        <div class="card-meta">
          <span class="card-price">${p.price_formatted}</span>
          <span class="card-stock ${stockClass}">${stockText}</span>
        </div>
        <div class="card-actions">
          <button class="btn-outline" onclick="openProductDetail(${p.id})">Details</button>
          <button class="btn-primary" onclick="addToCart(${p.id})" ${p.stock_qty <= 0 ? "disabled" : ""}>Add to Cart</button>
        </div>
      </div>
    `;
    grid.appendChild(card);
  });
}

// Product Details Modal
function openProductDetail(productId) {
  const p = state.db.products.find(x => x.id === productId);
  if (!p) return;

  const reviews = state.db.reviews.filter(r => r.product_id === productId);
  const avgRating = reviews.length ? (reviews.reduce((acc, r) => acc + r.rating, 0) / reviews.length).toFixed(1) : "No reviews yet";

  const modalBody = document.getElementById("detail-modal-body");
  modalBody.innerHTML = `
    <div style="display: flex; gap: 1.5rem; flex-wrap: wrap;">
      <img src="${p.image_url}" alt="${p.name}" style="width: 220px; height: 220px; object-fit: cover; border-radius: 8px;" />
      <div style="flex: 1; min-width: 240px;">
        <span class="status-badge" style="background: var(--primary-light); color: var(--primary);">${p.category}</span>
        <h2 style="margin: 0.5rem 0; font-size: 1.4rem;">${p.name}</h2>
        <div style="font-size: 1.5rem; font-weight: 800; color: var(--primary); margin-bottom: 0.5rem;">${p.price_formatted}</div>
        <p style="color: var(--gray-700); font-size: 0.95rem; margin-bottom: 1rem;">${p.description}</p>
        <div style="font-size: 0.85rem; color: var(--gray-500); margin-bottom: 1rem;">
          <div><strong>Seller:</strong> ${p.seller_name || "Verified Merchant"}</div>
          <div><strong>Stock:</strong> ${p.stock_qty} available</div>
          <div><strong>Rating:</strong> ⭐ ${avgRating} (${reviews.length} reviews)</div>
        </div>
        <button class="nav-btn btn-primary" onclick="addToCart(${p.id}); closeModal('detail-modal');" style="width: 100%;" ${p.stock_qty <= 0 ? "disabled" : ""}>
          ${p.stock_qty <= 0 ? "Out of Stock" : "Add to Cart"}
        </button>
      </div>
    </div>

    <hr style="margin: 1.5rem 0; border: none; border-top: 1px solid var(--gray-300);" />

    <h3>Customer Reviews (${reviews.length})</h3>
    <div style="margin-top: 1rem; display: flex; flex-direction: column; gap: 0.75rem;">
      ${reviews.length === 0 ? `<p style="color: var(--gray-500);">No reviews written yet. Buyers with delivered orders can leave a review below!</p>` : ""}
      ${reviews.map(r => `
        <div style="background: var(--gray-100); padding: 0.85rem; border-radius: 6px;">
          <div style="display: flex; justify-content: space-between; align-items: center; margin-bottom: 0.3rem;">
            <strong>${r.user_name}</strong>
            <span style="color: #f59e0b;">${"★".repeat(r.rating)}${"☆".repeat(5 - r.rating)}</span>
          </div>
          <p style="font-size: 0.9rem; color: var(--gray-700);">${r.comment}</p>
          <small style="color: var(--gray-500);">${r.created_at}</small>
        </div>
      `).join("")}
    </div>
  `;

  openModal("detail-modal");
}

// Shopping Cart Actions (F4)
function getCartItems() {
  const userId = state.currentUser ? state.currentUser.id : 0;
  if (!state.db.cart[userId]) {
    state.db.cart[userId] = [];
  }
  return state.db.cart[userId];
}

function updateCartCount() {
  const items = getCartItems();
  const count = items.reduce((sum, it) => sum + it.quantity, 0);
  document.getElementById("cart-count").innerText = count;
}

function addToCart(productId) {
  if (!state.currentUser) {
    showToast("Please select or log in to an account first.", "danger");
    return;
  }
  const prod = state.db.products.find(p => p.id === productId);
  if (!prod || prod.stock_qty <= 0) {
    showToast("Product is out of stock", "danger");
    return;
  }

  const items = getCartItems();
  const existing = items.find(it => it.product_id === productId);

  if (existing) {
    if (existing.quantity + 1 > prod.stock_qty) {
      showToast(`Cannot add more than available stock (${prod.stock_qty})`, "warning");
      return;
    }
    existing.quantity += 1;
  } else {
    items.push({
      id: Date.now(),
      product_id: productId,
      product_name: prod.name,
      price_cents: prod.price_cents,
      unit_price_formatted: prod.price_formatted,
      product_image_url: prod.image_url,
      quantity: 1
    });
  }

  state.save();
  updateCartCount();
  showToast(`Added '${prod.name}' to cart!`);
}

function updateCartItemQuantity(productId, delta) {
  const items = getCartItems();
  const existing = items.find(it => it.product_id === productId);
  if (!existing) return;

  const prod = state.db.products.find(p => p.id === productId);
  const newQty = existing.quantity + delta;

  if (newQty <= 0) {
    state.db.cart[state.currentUser.id] = items.filter(it => it.product_id !== productId);
  } else {
    if (prod && newQty > prod.stock_qty) {
      showToast("Cannot exceed available inventory", "warning");
      return;
    }
    existing.quantity = newQty;
  }

  state.save();
  updateCartCount();
  renderCartModal();
}

function renderCartModal() {
  const items = getCartItems();
  const container = document.getElementById("cart-items-container");
  const totalEl = document.getElementById("cart-running-total");
  const checkoutBtn = document.getElementById("cart-checkout-btn");

  container.innerHTML = "";
  let totalCents = 0;

  if (items.length === 0) {
    container.innerHTML = `<p style="text-align: center; color: var(--gray-500); padding: 2rem;">Your cart is empty.</p>`;
    totalEl.innerText = "₹0.00";
    checkoutBtn.disabled = true;
    return;
  }

  checkoutBtn.disabled = false;
  items.forEach(it => {
    const subtotalCents = it.price_cents * it.quantity;
    totalCents += subtotalCents;
    const subtotalFormatted = "₹" + (subtotalCents / 100).toFixed(2);

    const row = document.createElement("div");
    row.style.cssText = "display: flex; align-items: center; justify-content: space-between; padding: 0.75rem 0; border-bottom: 1px solid var(--gray-200);";
    row.innerHTML = `
      <div style="display: flex; align-items: center; gap: 0.75rem;">
        <img src="${it.product_image_url}" style="width: 50px; height: 50px; object-fit: cover; border-radius: 6px;" />
        <div>
          <div style="font-weight: 600; font-size: 0.95rem;">${it.product_name}</div>
          <div style="color: var(--gray-500); font-size: 0.85rem;">${it.unit_price_formatted} each</div>
        </div>
      </div>
      <div style="display: flex; align-items: center; gap: 1rem;">
        <div style="display: flex; align-items: center; border: 1px solid var(--gray-300); border-radius: 4px;">
          <button style="border: none; background: none; padding: 0.25rem 0.5rem; cursor: pointer;" onclick="updateCartItemQuantity(${it.product_id}, -1)">-</button>
          <span style="padding: 0 0.5rem; font-weight: 600;">${it.quantity}</span>
          <button style="border: none; background: none; padding: 0.25rem 0.5rem; cursor: pointer;" onclick="updateCartItemQuantity(${it.product_id}, 1)">+</button>
        </div>
        <span style="font-weight: 700; width: 70px; text-align: right;">${subtotalFormatted}</span>
      </div>
    `;
    container.appendChild(row);
  });

  totalEl.innerText = "₹" + (totalCents / 100).toFixed(2);
}

// Checkout & Mock Payment (F5)
function openCheckoutModal() {
  closeModal("cart-modal");
  const items = getCartItems();
  const totalCents = items.reduce((sum, it) => sum + (it.price_cents * it.quantity), 0);
  document.getElementById("checkout-amount").innerText = "₹" + (totalCents / 100).toFixed(2);
  openModal("checkout-modal");
}

function processCheckout(event) {
  event.preventDefault();
  const address = document.getElementById("shipping-address").value.trim();
  if (!address) {
    showToast("Please enter a valid shipping address", "danger");
    return;
  }

  const items = getCartItems();
  if (items.length === 0) return;

  const totalCents = items.reduce((sum, it) => sum + (it.price_cents * it.quantity), 0);

  // Decrement inventory in mock store
  for (const it of items) {
    const p = state.db.products.find(x => x.id === it.product_id);
    if (p) {
      p.stock_qty = Math.max(0, p.stock_qty - it.quantity);
    }
  }

  // Create Order (Status PENDING -> CONFIRMED via mock payment)
  const newOrder = {
    id: state.db.orders.length + 1,
    buyer_id: state.currentUser.id,
    buyer_name: state.currentUser.name,
    buyer_email: state.currentUser.email,
    status: "CONFIRMED",
    total_amount_cents: totalCents,
    total_amount_formatted: "₹" + (totalCents / 100).toFixed(2),
    created_at: new Date().toISOString().replace("T", " ").substring(0, 19),
    items: items.map(it => ({
      id: Date.now() + Math.floor(Math.random() * 1000),
      product_id: it.product_id,
      product_name: it.product_name,
      quantity: it.quantity,
      unit_price_cents: it.price_cents,
      unit_price_formatted: it.unit_price_formatted,
      seller_id: 2
    }))
  };

  state.db.orders.unshift(newOrder);
  state.db.cart[state.currentUser.id] = [];
  state.save();

  closeModal("checkout-modal");
  updateCartCount();
  showToast("Mock payment confirmed! Order #" + newOrder.id + " placed successfully.", "success");
  switchTab("orders");
}

// Buyer Order History (F6 & O2)
function renderOrders() {
  const container = document.getElementById("orders-list-container");
  container.innerHTML = "";

  const userOrders = state.db.orders.filter(o => o.buyer_id === state.currentUser.id);

  if (userOrders.length === 0) {
    container.innerHTML = `<div style="text-align: center; padding: 3rem; color: var(--gray-500);">
      <h3>No past orders found for your account.</h3>
      <p>Browse products and place an order with mock checkout!</p>
    </div>`;
    return;
  }

  userOrders.forEach(ord => {
    const card = document.createElement("div");
    card.className = "stat-card";
    card.style.marginBottom = "1.5rem";

    card.innerHTML = `
      <div style="display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid var(--gray-200); padding-bottom: 0.75rem; margin-bottom: 0.75rem;">
        <div>
          <strong>Order #${ord.id}</strong>
          <span style="color: var(--gray-500); font-size: 0.85rem; margin-left: 0.5rem;">${ord.created_at}</span>
        </div>
        <div style="display: flex; align-items: center; gap: 0.75rem;">
          <span class="status-badge status-${ord.status}">${ord.status}</span>
          <strong style="color: var(--primary);">${ord.total_amount_formatted}</strong>
        </div>
      </div>
      <div>
        ${ord.items.map(it => `
          <div style="display: flex; justify-content: space-between; align-items: center; padding: 0.4rem 0;">
            <div>
              <span>${it.product_name}</span>
              <small style="color: var(--gray-500);"> x ${it.quantity}</small>
            </div>
            <div style="display: flex; align-items: center; gap: 0.75rem;">
              <span>${it.unit_price_formatted}</span>
              ${ord.status === "DELIVERED" ? `
                <button class="nav-btn btn-outline" style="padding: 0.2rem 0.5rem; font-size: 0.75rem;" onclick="openReviewModal(${it.product_id}, '${it.product_name.replace(/'/g, "\\'")}')">Review</button>
              ` : ""}
            </div>
          </div>
        `).join("")}
      </div>
    `;
    container.appendChild(card);
  });
}

// Review Submission (F8)
let currentReviewProductId = null;
function openReviewModal(productId, productName) {
  currentReviewProductId = productId;
  document.getElementById("review-product-name").innerText = productName;
  document.getElementById("review-comment").value = "";
  document.getElementById("review-rating").value = "5";
  openModal("review-modal");
}

function submitReview(event) {
  event.preventDefault();
  if (!currentReviewProductId) return;

  const rating = Number(document.getElementById("review-rating").value);
  const comment = document.getElementById("review-comment").value.trim();

  if (!comment) {
    showToast("Please write a review comment", "danger");
    return;
  }

  const newReview = {
    id: state.db.reviews.length + 1,
    product_id: currentReviewProductId,
    user_id: state.currentUser.id,
    user_name: state.currentUser.name,
    rating: rating,
    comment: comment,
    created_at: new Date().toISOString().split("T")[0]
  };

  state.db.reviews.unshift(newReview);
  state.save();

  closeModal("review-modal");
  showToast("Thank you! Your product review has been submitted.", "success");
}

// Seller Dashboard (F2, O3, O2)
function renderSellerDashboard() {
  const sellerId = state.currentUser.id;
  const listings = state.db.products.filter(p => p.seller_id === sellerId);
  
  // Calculate seller orders & revenue
  let sellerOrders = [];
  let revenueCents = 0;
  state.db.orders.forEach(o => {
    const hasSellerItem = o.items.some(it => it.seller_id === sellerId);
    if (hasSellerItem) {
      sellerOrders.push(o);
      o.items.forEach(it => {
        if (it.seller_id === sellerId) {
          revenueCents += it.unit_price_cents * it.quantity;
        }
      });
    }
  });

  document.getElementById("seller-stat-listings").innerText = listings.length;
  document.getElementById("seller-stat-orders").innerText = sellerOrders.length;
  document.getElementById("seller-stat-revenue").innerText = "₹" + (revenueCents / 100).toFixed(2);

  // Render listings table
  const tbody = document.getElementById("seller-listings-tbody");
  tbody.innerHTML = "";
  listings.forEach(p => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td><img src="${p.image_url}" style="width: 40px; height: 40px; object-fit: cover; border-radius: 4px;" /></td>
      <td><strong>${p.name}</strong></td>
      <td>${p.category}</td>
      <td>${p.price_formatted}</td>
      <td>${p.stock_qty}</td>
      <td>
        <button class="nav-btn btn-outline" style="padding: 0.25rem 0.5rem; font-size: 0.75rem;" onclick="openEditProductModal(${p.id})">Edit</button>
        <button class="nav-btn btn-outline" style="padding: 0.25rem 0.5rem; font-size: 0.75rem; color: var(--danger);" onclick="deleteProduct(${p.id})">Delete</button>
      </td>
    `;
    tbody.appendChild(tr);
  });

  // Render incoming seller orders
  const ordersTbody = document.getElementById("seller-orders-tbody");
  ordersTbody.innerHTML = "";
  sellerOrders.forEach(o => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>#${o.id}</td>
      <td>${o.buyer_name}</td>
      <td>${o.created_at}</td>
      <td>${o.total_amount_formatted}</td>
      <td>
        <select onchange="updateOrderStatus(${o.id}, this.value)" style="padding: 0.25rem; border-radius: 4px;">
          <option value="PENDING" ${o.status === "PENDING" ? "selected" : ""}>PENDING</option>
          <option value="CONFIRMED" ${o.status === "CONFIRMED" ? "selected" : ""}>CONFIRMED</option>
          <option value="SHIPPED" ${o.status === "SHIPPED" ? "selected" : ""}>SHIPPED</option>
          <option value="DELIVERED" ${o.status === "DELIVERED" ? "selected" : ""}>DELIVERED</option>
          <option value="CANCELLED" ${o.status === "CANCELLED" ? "selected" : ""}>CANCELLED</option>
        </select>
      </td>
    `;
    ordersTbody.appendChild(tr);
  });
}

function updateOrderStatus(orderId, newStatus) {
  const ord = state.db.orders.find(o => o.id === orderId);
  if (ord) {
    ord.status = newStatus;
    state.save();
    showToast(`Order #${orderId} updated to ${newStatus}`);
    renderSellerDashboard();
  }
}

function saveProductListing(event) {
  event.preventDefault();
  const id = document.getElementById("product-form-id").value;
  const name = document.getElementById("product-form-name").value.trim();
  const category = document.getElementById("product-form-category").value.trim();
  const priceDollars = parseFloat(document.getElementById("product-form-price").value);
  const stock = parseInt(document.getElementById("product-form-stock").value, 10);
  const desc = document.getElementById("product-form-desc").value.trim();
  const img = document.getElementById("product-form-image").value.trim() || "https://images.unsplash.com/photo-1523275335684-37898b6baf30?w=500&q=80";

  if (!name || isNaN(priceDollars) || priceDollars <= 0 || isNaN(stock) || stock < 0) {
    showToast("Please fill in valid listing details", "danger");
    return;
  }

  const priceCents = Math.round(priceDollars * 100);
  const priceFormatted = "₹" + priceDollars.toFixed(2);

  if (id) {
    // Edit existing
    const p = state.db.products.find(x => x.id === Number(id));
    if (p) {
      p.name = name;
      p.category = category;
      p.price_cents = priceCents;
      p.price_dollars = priceDollars;
      p.price_formatted = priceFormatted;
      p.stock_qty = stock;
      p.description = desc;
      p.image_url = img;
    }
  } else {
    // Create new
    const newId = state.db.products.length ? Math.max(...state.db.products.map(p => p.id)) + 1 : 1;
    state.db.products.unshift({
      id: newId,
      seller_id: state.currentUser.id,
      seller_name: state.currentUser.name,
      name, category,
      price_cents: priceCents,
      price_dollars: priceDollars,
      price_formatted: priceFormatted,
      stock_qty: stock,
      description: desc,
      image_url: img
    });
  }

  state.save();
  closeModal("product-modal");
  showToast("Product listing saved successfully!", "success");
  renderSellerDashboard();
  renderProducts();
}

function openCreateProductModal() {
  document.getElementById("product-form-id").value = "";
  document.getElementById("product-form-name").value = "";
  document.getElementById("product-form-category").value = "Electronics";
  document.getElementById("product-form-price").value = "";
  document.getElementById("product-form-stock").value = "10";
  document.getElementById("product-form-desc").value = "";
  document.getElementById("product-form-image").value = "";
  document.getElementById("product-modal-title").innerText = "Create New Listing";
  openModal("product-modal");
}

function openEditProductModal(productId) {
  const p = state.db.products.find(x => x.id === productId);
  if (!p) return;

  document.getElementById("product-form-id").value = p.id;
  document.getElementById("product-form-name").value = p.name;
  document.getElementById("product-form-category").value = p.category;
  document.getElementById("product-form-price").value = (p.price_cents / 100).toFixed(2);
  document.getElementById("product-form-stock").value = p.stock_qty;
  document.getElementById("product-form-desc").value = p.description;
  document.getElementById("product-form-image").value = p.image_url;
  document.getElementById("product-modal-title").innerText = "Edit Listing";
  openModal("product-modal");
}

function deleteProduct(productId) {
  if (!confirm("Are you sure you want to delete this listing?")) return;
  state.db.products = state.db.products.filter(p => p.id !== productId);
  state.save();
  showToast("Product listing deleted");
  renderSellerDashboard();
  renderProducts();
  if (state.activeTab === "admin") renderAdminPanel();
}

// Admin Panel (F7)
function renderAdminPanel() {
  const totalUsers = state.db.users.length;
  const totalSellers = state.db.users.filter(u => u.role === "SELLER").length;
  const totalBuyers = state.db.users.filter(u => u.role === "BUYER").length;
  const totalProducts = state.db.products.length;
  const totalOrders = state.db.orders.length;
  const totalRevenueCents = state.db.orders.reduce((sum, o) => sum + o.total_amount_cents, 0);

  document.getElementById("admin-stat-users").innerText = totalUsers;
  document.getElementById("admin-stat-sellers").innerText = totalSellers;
  document.getElementById("admin-stat-buyers").innerText = totalBuyers;
  document.getElementById("admin-stat-products").innerText = totalProducts;
  document.getElementById("admin-stat-orders").innerText = totalOrders;
  document.getElementById("admin-stat-revenue").innerText = "₹" + (totalRevenueCents / 100).toFixed(2);

  // Render all users table
  const userTbody = document.getElementById("admin-users-tbody");
  userTbody.innerHTML = "";
  state.db.users.forEach(u => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>${u.id}</td>
      <td><strong>${u.name}</strong></td>
      <td>${u.email}</td>
      <td><span class="status-badge" style="background: var(--primary-light); color: var(--primary);">${u.role}</span></td>
    `;
    userTbody.appendChild(tr);
  });

  // Render all platform orders
  const ordersTbody = document.getElementById("admin-orders-tbody");
  ordersTbody.innerHTML = "";
  state.db.orders.forEach(o => {
    const tr = document.createElement("tr");
    tr.innerHTML = `
      <td>#${o.id}</td>
      <td>${o.buyer_name} (${o.buyer_email})</td>
      <td>${o.created_at}</td>
      <td>${o.total_amount_formatted}</td>
      <td><span class="status-badge status-${o.status}">${o.status}</span></td>
    `;
    ordersTbody.appendChild(tr);
  });
}

// Modal Helpers
function openModal(id) {
  const modal = document.getElementById(id);
  if (modal) modal.classList.add("open");
}

function closeModal(id) {
  const modal = document.getElementById(id);
  if (modal) modal.classList.remove("open");
}

// Global Event Handlers Initialization
document.addEventListener("DOMContentLoaded", () => {
  checkBackendHealth();
  updateUserUI();
  updateCartCount();
  renderProducts();

  // Category filter chips
  document.querySelectorAll(".cat-chip").forEach(chip => {
    chip.addEventListener("click", () => {
      document.querySelectorAll(".cat-chip").forEach(c => c.classList.remove("active"));
      chip.classList.add("active");
      state.activeCategory = chip.dataset.category;
      renderProducts();
    });
  });

  // Search input with debounce
  const searchInput = document.getElementById("search-input");
  searchInput.addEventListener("input", (e) => {
    state.searchKeyword = e.target.value;
    renderProducts();
  });

  // Close modals on backdrop click
  document.querySelectorAll(".modal-backdrop").forEach(modal => {
    modal.addEventListener("click", (e) => {
      if (e.target === modal) {
        modal.classList.remove("open");
      }
    });
  });
});
