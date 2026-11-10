const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
};

export async function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export async function onRequestGet() {
  return Response.json(
    {
      success: true,
      status: "UP",
      db: "UP",
      data: {
        status: "UP",
        db: "UP",
      },
      platform: "Cloudflare Pages Functions",
      timestamp: new Date().toISOString(),
      error: null,
    },
    { status: 200, headers: CORS_HEADERS }
  );
}
