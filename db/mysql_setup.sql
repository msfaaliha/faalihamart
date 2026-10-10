-- ============================================================
-- FaalihaMart MySQL Database Setup & Seed Script
-- Target: MySQL 8.0+ / MariaDB 10.5+
-- ============================================================

CREATE DATABASE IF NOT EXISTS faalihamart
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE faalihamart;

-- 1. Schema Migrations Table
CREATE TABLE IF NOT EXISTS schema_migrations (
    version INT PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 2. Users Table
CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(255) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    role ENUM('BUYER', 'SELLER', 'ADMIN') NOT NULL DEFAULT 'BUYER',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_users_email (email)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 3. Products Table
CREATE TABLE IF NOT EXISTS products (
    id INT AUTO_INCREMENT PRIMARY KEY,
    seller_id INT NOT NULL,
    name VARCHAR(255) NOT NULL,
    description TEXT,
    price_cents BIGINT NOT NULL,
    stock_qty INT NOT NULL DEFAULT 0,
    category VARCHAR(100) NOT NULL,
    image_url TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (seller_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_products_seller_id (seller_id),
    INDEX idx_products_category (category)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 4. Orders Table
CREATE TABLE IF NOT EXISTS orders (
    id INT AUTO_INCREMENT PRIMARY KEY,
    buyer_id INT NOT NULL,
    status ENUM('PENDING', 'CONFIRMED', 'SHIPPED', 'DELIVERED', 'CANCELLED') NOT NULL DEFAULT 'PENDING',
    total_amount_cents BIGINT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (buyer_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_orders_buyer_id (buyer_id),
    INDEX idx_orders_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 5. Order Items Table
CREATE TABLE IF NOT EXISTS order_items (
    id INT AUTO_INCREMENT PRIMARY KEY,
    order_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 1,
    unit_price_cents BIGINT NOT NULL,
    FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE RESTRICT,
    INDEX idx_order_items_order_id (order_id),
    INDEX idx_order_items_product_id (product_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 6. Cart Items Table
CREATE TABLE IF NOT EXISTS cart_items (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity INT NOT NULL DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_cart_user_product (user_id, product_id),
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,
    INDEX idx_cart_items_user_id (user_id),
    INDEX idx_cart_items_product_id (product_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 7. Reviews Table
CREATE TABLE IF NOT EXISTS reviews (
    id INT AUTO_INCREMENT PRIMARY KEY,
    product_id INT NOT NULL,
    user_id INT NOT NULL,
    rating TINYINT NOT NULL CHECK (rating >= 1 AND rating <= 5),
    comment TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,
    INDEX idx_reviews_product_id (product_id),
    INDEX idx_reviews_user_id (user_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ============================================================
-- Seed Initial Accounts, Products, Orders, and Verified Reviews
-- ============================================================

-- Users (Argon2id hashed passwords matching seed.sql)
INSERT IGNORE INTO users (id, name, email, password_hash, role) VALUES
(1, 'Admin Faaliha', 'admin@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'ADMIN'),
(2, 'Apex Tech Gear', 'techseller@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'SELLER'),
(3, 'Urban Trendz', 'styleseller@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'SELLER'),
(4, 'Alice Smith', 'alice@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'BUYER'),
(5, 'Bob Jones', 'bob@faalihamart.com', '$argon2id$v=19$m=65536,t=2,p=1$X1NFRURfU0FMVA$eI1Kz129yF793YxWcR2a9A8k2dE3fG4h5j6k7l8m9n0', 'BUYER');

-- Products
INSERT IGNORE INTO products (id, seller_id, name, description, price_cents, stock_qty, category, image_url) VALUES
(1, 2, 'Noise-Cancelling Wireless Headphones', 'Premium over-ear acoustic headphones with 40-hour battery life and spatial audio.', 14999, 25, 'Electronics', 'https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=500&q=80'),
(2, 2, 'Mechanical Gaming Keyboard RGB', 'Custom mechanical switches, per-key RGB backlighting, and detachable braided USB-C cable.', 8999, 14, 'Electronics', 'https://images.unsplash.com/photo-1587829741301-dc798b83add3?w=500&q=80'),
(3, 2, 'Ultra-Slim 4K USB-C Monitor 27"', 'Crisp IPS panel, 99% sRGB color gamut, HDR400, and 65W power delivery.', 29999, 8, 'Electronics', 'https://images.unsplash.com/photo-1527443224154-c4a3942d3acf?w=500&q=80'),
(4, 3, 'Classic Organic Cotton Hoodie', 'Heavyweight brushed fleece hoodie tailored from 100% sustainable organic cotton.', 4999, 40, 'Apparel', 'https://images.unsplash.com/photo-1556905055-8f358a7a47b2?w=500&q=80'),
(5, 3, 'Waterproof Commuter Backpack', 'Minimalist weatherproof everyday backpack featuring a padded 16-inch laptop compartment.', 7499, 18, 'Apparel', 'https://images.unsplash.com/photo-1553062407-98eeb64c6a62?w=500&q=80'),
(6, 3, 'Aromatic Soy Wax Candle Set', 'Hand-poured triple-scented essential oil candles with cedarwood, vanilla, and amber notes.', 2499, 50, 'Home & Living', 'https://images.unsplash.com/photo-1603006905003-be475563bc59?w=500&q=80'),
(7, 2, 'Ergonomic Vertical Wireless Mouse', 'Designed to reduce forearm strain, featuring optical tracking and silent clicks.', 3499, 30, 'Electronics', 'https://images.unsplash.com/photo-1615663245857-ac93bb7c39e7?w=500&q=80'),
(8, 3, 'Insulated Stainless Steel Tumbler 32oz', 'Double-wall vacuum insulation keeps drinks cold for 24h or piping hot for 12h.', 1999, 65, 'Home & Living', 'https://images.unsplash.com/photo-1514432324607-a09d9b4aefdd?w=500&q=80');

-- Orders (Workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED)
INSERT IGNORE INTO orders (id, buyer_id, status, total_amount_cents) VALUES
(1, 4, 'DELIVERED', 19998),
(2, 4, 'SHIPPED', 8999),
(3, 5, 'CONFIRMED', 14999);

-- Order Items
INSERT IGNORE INTO order_items (id, order_id, product_id, quantity, unit_price_cents) VALUES
(1, 1, 1, 1, 14999),
(2, 1, 4, 1, 4999),
(3, 2, 2, 1, 8999),
(4, 3, 1, 1, 14999);

-- Verified Reviews
INSERT IGNORE INTO reviews (id, product_id, user_id, rating, comment) VALUES
(1, 1, 4, 5, 'Absolutely incredible sound quality and comfortable padding for long work sessions.'),
(2, 4, 4, 4, 'Very soft material and true to size fit. Highly recommended!'),
(3, 2, 5, 5, 'Keys feel tactile and responsive. Great build quality!');
