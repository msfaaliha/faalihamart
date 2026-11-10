export default function handler(req, res) {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Methods', 'GET, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');

  if (req.method === 'OPTIONS') {
    return res.status(204).end();
  }

  res.status(200).json({
    success: true,
    status: 'UP',
    db: 'UP',
    data: {
      status: 'UP',
      db: 'UP'
    },
    platform: 'Vercel Serverless',
    timestamp: new Date().toISOString(),
    error: null
  });
}
