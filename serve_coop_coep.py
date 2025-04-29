import http.server
import socketserver

PORT = 8000

class COOPCOEPHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        http.server.SimpleHTTPRequestHandler.end_headers(self)

if __name__ == "__main__":
    with socketserver.TCPServer(("", PORT), COOPCOEPHandler) as httpd:
        print(f"Serving at http://localhost:{PORT} with COOP/COEP headers")
        httpd.serve_forever()