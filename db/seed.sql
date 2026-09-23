-- FaalihaMart Seed Data
-- Seed accounts:
-- 1. Admin: admin@faalihamart.com / AdminPass123!
-- 2. Seller 1: techseller@faalihamart.com / SellerPass123!
-- 3. Seller 2: styleseller@faalihamart.com / SellerPass123!
-- 4. Buyer 1: alice@faalihamart.com / BuyerPass123!
-- 5. Buyer 2: bob@faalihamart.com / BuyerPass123!

-- Passwords hashed using Argon2id / libsodium formatted string
INSERT OR IGNORE INTO users (id, name, email, password_hash, role) VALUES
(1, 'Admin Faaliha', 'admin@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'ADMIN'),
(2, 'Apex Tech Gear', 'techseller@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'SELLER'),
(3, 'Urban Trendz', 'styleseller@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'SELLER'),
(4, 'Alice Smith', 'alice@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'BUYER'),
(5, 'Bob Jones', 'bob@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'BUYER');

-- Products: Monetary values stored as BIGINT cents ($149.99 = 14999)
INSERT OR IGNORE INTO products (id, seller_id, name, description, price_cents, stock_qty, category, image_url) VALUES
(1, 2, 'Noise-Cancelling Wireless Headphones', 'Premium over-ear acoustic headphones with 40-hour battery life and spatial audio.', 14999, 25, 'Electronics', 'https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=500&q=80'),
(2, 2, 'Mechanical Gaming Keyboard RGB', 'Custom mechanical switches, per-key RGB backlighting, and detachable braided USB-C cable.', 8999, 14, 'Electronics', 'https://images.unsplash.com/photo-1587829741301-dc798b83add3?w=500&q=80'),
(3, 2, 'Ultra-Slim 4K USB-C Monitor 27"', 'Crisp IPS panel, 99% sRGB color gamut, HDR400, and 65W power delivery.', 29999, 8, 'Electronics', 'https://images.unsplash.com/photo-1527443224154-c4a3942d3acf?w=500&q=80'),
(4, 3, 'Classic Organic Cotton Hoodie', 'Heavyweight brushed fleece hoodie tailored from 100% sustainable organic cotton.', 4999, 40, 'Apparel', 'https://images.unsplash.com/photo-1556905055-8f358a7a47b2?w=500&q=80'),
(5, 3, 'Waterproof Commuter Backpack', 'Minimalist weatherproof everyday backpack featuring a padded 16-inch laptop compartment.', 7499, 18, 'Apparel', 'https://images.unsplash.com/photo-1553062407-98eeb64c6a62?w=500&q=80'),
(6, 3, 'Aromatic Soy Wax Candle Set', 'Hand-poured triple-scented essential oil candles with cedarwood, vanilla, and amber notes.', 2499, 50, 'Home & Living', 'https://images.unsplash.com/photo-1603006905003-be475563bc59?w=500&q=80'),
(7, 2, 'Ergonomic Vertical Wireless Mouse', 'Designed to reduce forearm strain, featuring optical tracking and silent clicks.', 3499, 30, 'Electronics', 'https://images.unsplash.com/photo-1615663245857-ac93bb7c39e7?w=500&q=80'),
(8, 3, 'Insulated Stainless Steel Tumbler 32oz', 'Double-wall vacuum insulation keeps drinks cold for 24h or piping hot for 12h.', 1999, 65, 'Home & Living', 'https://images.unsplash.com/photo-1514432324607-a09d9b4aefdd?w=500&q=80');

-- Orders & status workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED
INSERT OR IGNORE INTO orders (id, buyer_id, status, total_amount_cents) VALUES
(1, 4, 'DELIVERED', 19998),
(2, 4, 'SHIPPED', 8999),
(3, 5, 'CONFIRMED', 14999);

INSERT OR IGNORE INTO order_items (id, order_id, product_id, quantity, unit_price_cents) VALUES
(1, 1, 1, 1, 14999),
(2, 1, 4, 1, 4999),
(3, 2, 2, 1, 8999),
(4, 3, 1, 1, 14999);

-- Product reviews & star ratings on completed orders
INSERT OR IGNORE INTO reviews (id, product_id, user_id, rating, comment) VALUES
(1, 1, 4, 5, 'Absolutely incredible sound quality and comfortable padding for long work sessions.'),
(2, 4, 4, 4, 'Very soft material and true to size fit. Highly recommended!'),
(3, 2, 5, 5, 'Keys feel tactile and responsive. Great build quality!');
