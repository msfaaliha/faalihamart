const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
  "Content-Type": "application/json; charset=utf-8"
};

function getFaqReply(userMsg) {
  const q = (userMsg || "").toLowerCase();

  if (q.includes("headphone") || q.includes("audio") || q.includes("earphone") || q.includes("noise-cancelling")) {
    return "We have the 'Noise-Cancelling Wireless Headphones' by Apex Tech Gear for ₹149.99 (40-hour battery life, spatial audio) in our Electronics category.";
  }
  if (q.includes("keyboard") || q.includes("rgb") || q.includes("switch")) {
    return "The 'Mechanical Gaming Keyboard RGB' is available for ₹89.99 featuring custom mechanical switches and per-key RGB backlighting.";
  }
  if (q.includes("monitor") || q.includes("screen") || q.includes("4k")) {
    return "Our 'Ultra-Slim 4K USB-C Monitor 27\"' is ₹299.99 with IPS panel, 99% sRGB color gamut, HDR400, and 65W power delivery.";
  }
  if (q.includes("hoodie") || q.includes("cotton") || q.includes("jacket")) {
    return "The 'Classic Organic Cotton Hoodie' is ₹49.99 tailored from 100% sustainable organic cotton in the Apparel category.";
  }
  if (q.includes("backpack") || q.includes("bag") || q.includes("laptop")) {
    return "The 'Waterproof Commuter Backpack' is ₹74.99 with weatherproof construction and a 16-inch laptop compartment.";
  }
  if (q.includes("candle") || q.includes("tumbler") || q.includes("home")) {
    return "In Home & Living, check out our Soy Wax Candle Set (₹24.99) and Insulated Stainless Steel Tumbler (₹19.99).";
  }
  if (q.includes("track") || q.includes("status") || q.includes("order") || q.includes("workflow")) {
    return "FaalihaMart orders follow a 4-step workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. Check the 'My Orders' tab to inspect details!";
  }
  if (q.includes("return") || q.includes("refund") || q.includes("exchange")) {
    return "FaalihaMart offers a 7-day hassle-free return window on delivered orders. You can also submit reviews and ratings once delivered.";
  }
  if (q.includes("payment") || q.includes("pay") || q.includes("checkout") || q.includes("upi")) {
    return "FaalihaMart checkout uses a simulated mock payment confirmation step (Mock Credit Card or Mock UPI). No real card is billed.";
  }
  if (q.includes("sell") || q.includes("seller") || q.includes("listing")) {
    return "To list products, switch to Seller role in the top header. You'll gain access to the Seller Dashboard for listing CRUD and revenue analytics.";
  }
  if (q.includes("review") || q.includes("rating") || q.includes("star")) {
    return "Buyers who have purchased a product can submit a 1 to 5 star rating and comment after the order reaches DELIVERED status.";
  }

  return "Hello! Welcome to FaalihaMart. I can assist with products in Electronics, Apparel, and Home & Living, order status tracking (PENDING -> CONFIRMED -> SHIPPED -> DELIVERED), returns, mock payment, or seller registration. How may I help you?";
}

async function callGeminiIfConfigured(userMsg, env) {
  const apiKey = env?.GEMINI_API_KEY;
  if (!apiKey || apiKey === "your_gemini_api_key_here") {
    return null;
  }

  const model = env.GEMINI_MODEL || "gemini-2.5-flash";
  const url = `https://generativelanguage.googleapis.com/v1beta/models/${encodeURIComponent(model)}:generateContent?key=${encodeURIComponent(apiKey)}`;
  const systemInstruction =
    "You are the official FaalihaMart AI Shopping & Support Assistant. " +
    "FaalihaMart is a multi-seller e-commerce marketplace featuring Electronics, Apparel, and Home & Living products with prices in INR (₹). " +
    "Orders follow a lifecycle: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. " +
    "Checkout uses simulated mock payment verification (Mock Credit Card / Mock UPI). Reviews (1-5 stars) require a delivered order. " +
    "Always be concise, friendly, and helpful within the FaalihaMart marketplace domain.";

  try {
    const resp = await fetch(url, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        system_instruction: { parts: [{ text: systemInstruction }] },
        contents: [{ role: "user", parts: [{ text: userMsg }] }],
        generationConfig: { temperature: 0.3, maxOutputTokens: 300 }
      })
    });

    if (!resp.ok) return null;
    const data = await resp.json();
    const text = data?.candidates?.[0]?.content?.parts?.[0]?.text;
    return text || null;
  } catch {
    return null;
  }
}

export function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export async function onRequestPost(context) {
  let userMsg = "";
  try {
    const bodyText = await context.request.text();
    if (bodyText) {
      const parsed = JSON.parse(bodyText);
      userMsg = typeof parsed?.message === "string" ? parsed.message.trim() : "";
    }
  } catch {
    return new Response(
      JSON.stringify({
        success: false,
        data: null,
        error: { code: "BAD_REQUEST", message: "Invalid JSON request body" }
      }),
      { status: 400, headers: CORS_HEADERS }
    );
  }

  if (!userMsg || userMsg.length > 500) {
    return new Response(
      JSON.stringify({
        success: false,
        data: null,
        error: { code: "VALIDATION_ERROR", message: "Message must be between 1 and 500 characters" }
      }),
      { status: 400, headers: CORS_HEADERS }
    );
  }

  let provider = "cloudflare-pages";
  let reply = await callGeminiIfConfigured(userMsg, context.env);
  if (reply) {
    provider = "gemini-cloudflare";
  } else {
    reply = getFaqReply(userMsg);
  }

  return new Response(
    JSON.stringify({
      success: true,
      data: {
        reply,
        provider,
        cached: false
      },
      error: null
    }),
    { status: 200, headers: CORS_HEADERS }
  );
}
