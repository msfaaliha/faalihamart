export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    return res.status(204).end();
  }

  let userMsg = '';
  if (req.body) {
    if (typeof req.body === 'string') {
      try {
        const parsed = JSON.parse(req.body);
        userMsg = parsed.message || '';
      } catch (e) {
        userMsg = req.body;
      }
    } else {
      userMsg = req.body.message || '';
    }
  }

  const q = (userMsg || '').toLowerCase();
  let reply = "Hello! Welcome to FaalihaMart. I can assist with products in Electronics, Apparel, and Home & Living, order status tracking (PENDING -> CONFIRMED -> SHIPPED -> DELIVERED), returns, mock payment, or seller registration. How may I help you?";

  if (q.includes("headphone") || q.includes("audio") || q.includes("earphone")) {
    reply = "We have the 'Noise-Cancelling Wireless Headphones' by Apex Tech Gear for ₹149.99 (40-hour battery life, spatial audio) in our Electronics category.";
  } else if (q.includes("keyboard") || q.includes("rgb") || q.includes("switch")) {
    reply = "The 'Mechanical Gaming Keyboard RGB' is available for ₹89.99 featuring custom mechanical switches and per-key RGB backlighting.";
  } else if (q.includes("monitor") || q.includes("screen") || q.includes("4k")) {
    reply = "Our 'Ultra-Slim 4K USB-C Monitor 27\"' is ₹299.99 with IPS panel, 99% sRGB color gamut, HDR400, and 65W power delivery.";
  } else if (q.includes("hoodie") || q.includes("cotton") || q.includes("jacket")) {
    reply = "The 'Classic Organic Cotton Hoodie' is ₹49.99 tailored from 100% sustainable organic cotton in the Apparel category.";
  } else if (q.includes("backpack") || q.includes("bag") || q.includes("laptop")) {
    reply = "The 'Waterproof Commuter Backpack' is ₹74.99 with weatherproof construction and a 16-inch laptop compartment.";
  } else if (q.includes("candle") || q.includes("tumbler") || q.includes("home")) {
    reply = "In Home & Living, check out our Soy Wax Candle Set (₹24.99) and Insulated Stainless Steel Tumbler (₹19.99).";
  } else if (q.includes("track") || q.includes("status") || q.includes("order") || q.includes("workflow")) {
    reply = "FaalihaMart orders follow a 4-step workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. Check the 'My Orders' tab to inspect details!";
  } else if (q.includes("return") || q.includes("refund")) {
    reply = "FaalihaMart offers a 7-day hassle-free return window on delivered orders. You can also submit reviews and ratings once delivered.";
  } else if (q.includes("payment") || q.includes("pay") || q.includes("checkout")) {
    reply = "FaalihaMart checkout uses a simulated mock payment confirmation step (Mock Credit Card or Mock UPI). No real card is billed.";
  } else if (q.includes("sell") || q.includes("seller") || q.includes("listing")) {
    reply = "To list products, switch to Seller role in the top header. You'll gain access to the Seller Dashboard for listing CRUD and revenue analytics.";
  } else if (q.includes("review") || q.includes("rating") || q.includes("star")) {
    reply = "Buyers who have purchased a product can submit a 1 to 5 star rating and comment after the order reaches DELIVERED status.";
  }

  res.status(200).json({
    success: true,
    data: {
      reply,
      provider: "vercel-serverless",
      cached: false
    },
    error: null
  });
}
