const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
};

function getMockReply(userMsg) {
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
  if (q.includes("track") || q.includes("status") || q.includes("order") || q.includes("workflow") || q.includes("shipped") || q.includes("delivered")) {
    return "FaalihaMart orders follow a 4-step workflow: PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. Check the 'My Orders' tab to inspect details!";
  }
  if (q.includes("return") || q.includes("refund") || q.includes("exchange")) {
    return "FaalihaMart offers a 7-day hassle-free return window on delivered orders. You can also submit reviews and ratings once delivered.";
  }
  if (q.includes("payment") || q.includes("pay") || q.includes("checkout") || q.includes("mock")) {
    return "FaalihaMart checkout uses a simulated mock payment confirmation step (Mock Credit Card or Mock UPI). No real card is billed.";
  }
  if (q.includes("sell") || q.includes("seller") || q.includes("listing")) {
    return "To list products, switch to Seller role in the top header. You'll gain access to the Seller Dashboard for listing CRUD and revenue analytics.";
  }
  if (q.includes("review") || q.includes("star") || q.includes("rating")) {
    return "Buyers who have purchased a product can submit a 1 to 5 star rating and comment after the order reaches DELIVERED status.";
  }
  return "Hello! Welcome to FaalihaMart. I can assist with products in Electronics, Apparel, and Home & Living, order status tracking (PENDING -> CONFIRMED -> SHIPPED -> DELIVERED), returns, mock payment, or seller registration. How may I help you?";
}

export async function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export async function onRequestPost(context) {
  try {
    let userMsg = "";
    const contentType = context.request.headers.get("content-type") || "";
    if (contentType.includes("application/json")) {
      const body = await context.request.json();
      userMsg = body && body.message ? String(body.message) : "";
    } else {
      userMsg = await context.request.text();
    }

    if (userMsg.length > 500) {
      return Response.json(
        { success: false, data: null, error: { code: "VALIDATION_ERROR", message: "Message must not exceed 500 characters" } },
        { status: 400, headers: CORS_HEADERS }
      );
    }

    let reply = "";
    let provider = "cloudflare-faq";

    const apiKey = context.env && context.env.GEMINI_API_KEY;
    if (apiKey && userMsg.trim()) {
      try {
        const geminiResp = await fetch(
          `https://generativelanguage.googleapis.com/v1beta/models/gemini-2.5-flash:generateContent?key=${encodeURIComponent(apiKey)}`,
          {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({
              system_instruction: {
                parts: [{ text: "You are the helpful FaalihaMart AI Marketplace Assistant. Be concise. Currency is INR (₹). Order lifecycle is PENDING -> CONFIRMED -> SHIPPED -> DELIVERED. Checkout uses simulated mock payment." }],
              },
              contents: [{ role: "user", parts: [{ text: userMsg }] }],
            }),
          }
        );
        if (geminiResp.ok) {
          const data = await geminiResp.json();
          const candidate = data?.candidates?.[0]?.content?.parts?.[0]?.text;
          if (candidate) {
            reply = candidate;
            provider = "gemini";
          }
        }
      } catch (_) {
        // Degrade gracefully to deterministic FAQ engine
      }
    }

    if (!reply) {
      reply = getMockReply(userMsg);
    }

    return Response.json(
      {
        success: true,
        data: {
          reply,
          provider,
          cached: false,
        },
        error: null,
      },
      { status: 200, headers: CORS_HEADERS }
    );
  } catch (err) {
    return Response.json(
      { success: false, data: null, error: { code: "INTERNAL_ERROR", message: err.message || "Invalid request" } },
      { status: 400, headers: CORS_HEADERS }
    );
  }
}
