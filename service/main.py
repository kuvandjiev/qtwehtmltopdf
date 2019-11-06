from aiohttp import web

import asyncio
import logging
import aiofiles
import tempfile
import os


logger = logging.getLogger()

HOST = os.environ.get("PYTHON_SERVICE_HOST", "0.0.0.0")
PORT = int(os.environ.get("PYTHON_SERVICE_PORT", 5555))
QTWEHTMLTOPDFBIN = os.environ.get("QTWEHTMLTOPDFBIN", "/app/build/qtwehtmltopdf")
DISPLAY = os.environ.get("DISPLAY", ":99")
MAX_CLIENT_REQUEST_SIZE_BYTES = 5 * 1024**2  # 5mb


class RequestHandler:

    async def convert_to_pdf(self, request):
        raw_data = await request.read()
        temp_file = tempfile.NamedTemporaryFile('wb', delete=False)
        temp_file.close()

        async with aiofiles.open(temp_file.name, mode='wb') as f:
            await f.write(raw_data)

        cmd = " ".join(["XDG_RUNTIME_DIR=/tmp/ DISPLAY={}".format(DISPLAY), QTWEHTMLTOPDFBIN, temp_file.name])
        logger.info(cmd)

        proc = await asyncio.create_subprocess_shell(cmd, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE)
        result, stderr = await proc.communicate()

        if stderr:
            logger.error(stderr)

        return web.Response(body=result, content_type="application/pdf")


def setup_routes(app, handler):
    app.router.add_post('/topdf', handler.convert_to_pdf, name='topdf')


async def server():
    app = web.Application(client_max_size=MAX_CLIENT_REQUEST_SIZE_BYTES)
    handler = RequestHandler()
    setup_routes(app, handler)
    host, port = HOST, PORT
    return app, host, port


def main():
    if not os.path.isfile(QTWEHTMLTOPDFBIN):
        print(QTWEHTMLTOPDFBIN, "does not exist. Have you built it?")
        exit(1)

    logging.basicConfig(level=logging.DEBUG)
    loop = asyncio.get_event_loop()
    app, host, port = loop.run_until_complete(server())
    web.run_app(app, host=host, port=port)


if __name__ == '__main__':
    main()
