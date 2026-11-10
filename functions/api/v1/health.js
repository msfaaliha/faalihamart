const CORS_HEADERS = {
  "Access-Control-Allow-Origin": "*",
  "Access-Control-Allow-Methods": "GET, OPTIONS",
  "Access-Control-Allow-Headers": "Content-Type, Authorization",
  "Content-Type": "application/json; charset=utf-8"
};

export function onRequestOptions() {
  return new Response(null, { status: 204, headers: CORS_HEADERS });
}

export function onRequestGet() {
  const payload = {
    status: "UP",
    db: "UP",
    platform: "Cloudflare Pages",
    timestamp: new Date().toISOString(),
    success: true,
    data: {
      status: "UP",
      db: "UP",
      platform: "Cloudflare Pages"
    },
    error: null
  };

  return new Response(JSON.stringify(payload), {
    status: 200,
    headers: CORS_HEADERS
  });
}
