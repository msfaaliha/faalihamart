const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
};

export async function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export async function onRequestGet(context) {
  const hasGemini = Boolean(context.env && context.env.GEMINI_API_KEY);
  return Response.json(
    {
      success: true,
      data: {
        provider: hasGemini ? "gemini" : "cloudflare-faq",
        rate_limit_per_minute: 10,
        max_message_length: 500,
      },
      error: null,
    },
    { status: 200, headers: CORS_HEADERS }
  );
}
