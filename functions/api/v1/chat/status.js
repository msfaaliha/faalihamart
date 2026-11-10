const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
  "Content-Type": "application/json; charset=utf-8",
};

export function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export function onRequestGet(context) {
  const apiKey = context.env && context.env.GEMINI_API_KEY;
  const hasGemini = Boolean(apiKey && apiKey !== "your_gemini_api_key_here");
  return new Response(
    JSON.stringify({
      success: true,
      data: {
        provider: hasGemini ? "gemini-cloudflare" : "cloudflare-pages",
        rate_limit_per_minute: 10,
        max_message_length: 500,
      },
      error: null,
    }),
    { status: 200, headers: CORS_HEADERS }
  );
}
