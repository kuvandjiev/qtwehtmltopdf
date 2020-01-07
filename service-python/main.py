from aiohttp import web

import asyncio
import logging
import aiofiles
import tempfile
import os
import shlex


logger = logging.getLogger()

HOST = os.environ.get("PYTHON_SERVICE_HOST", "0.0.0.0")
PORT = int(os.environ.get("PYTHON_SERVICE_PORT", 5555))
QTWEHTMLTOPDFBIN = os.environ.get("QTWEHTMLTOPDFBIN", "/app/build/qtwehtmltopdf")
DISPLAY = os.environ.get("DISPLAY", ":99")
MAX_CLIENT_REQUEST_SIZE_BYTES = 5 * 1024**2  # 5mb


async def create_input_file(raw_data):
    temp_file = tempfile.NamedTemporaryFile('wb', delete=False)
    temp_file.close()

    async with aiofiles.open(temp_file.name, mode='wb') as f:
        await f.write(raw_data)
    return temp_file.name


async def convert_to_pdf(input_file_name):
    cmd = " ".join(["XDG_RUNTIME_DIR=/tmp/ DISPLAY={}".format(DISPLAY), QTWEHTMLTOPDFBIN, shlex.quote(input_file_name)])
    logger.info(cmd)
    proc = await asyncio.create_subprocess_shell(cmd, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE)
    result, stderr = await proc.communicate()
    if stderr:
        logger.error(stderr)
    return result


async def convert_pdf_to_jpeg(input_file_name):
    cmd = " ".join(["convert", "-quality", "100%", input_file_name, "-append", "jpeg:-", ])
    logger.info(cmd)
    proc = await asyncio.create_subprocess_shell(cmd, stdout=asyncio.subprocess.PIPE, stderr=asyncio.subprocess.PIPE)
    result, stderr = await proc.communicate()
    if stderr:
        logger.error(stderr)
    return result


class RequestHandler:

    async def convert_to_pdf(self, request):
        temp_input_file_name = await create_input_file(await request.read())
        result = await convert_to_pdf(input_file_name=temp_input_file_name)
        return web.Response(body=result, content_type="application/pdf")

    async def convert_to_jpeg(self, request):
        temp_input_file_name = await create_input_file(await request.read())
        pdf_data = await convert_to_pdf(input_file_name=temp_input_file_name)

        temp_input_file_name = await create_input_file(pdf_data)
        result = await convert_pdf_to_jpeg(input_file_name=temp_input_file_name)

        return web.Response(body=result, content_type="image/jpeg")

    async def convert_url_to_pdf(self, request):
        url = await request.read()
        result = await convert_to_pdf(input_file_name=url.decode())
        return web.Response(body=result, content_type="application/pdf")

    async def convert_url_to_jpeg(self, request):
        url = await request.read()
        pdf_data = await convert_to_pdf(input_file_name=url.decode())

        temp_input_file_name = await create_input_file(pdf_data)
        result = await convert_pdf_to_jpeg(input_file_name=temp_input_file_name)

        return web.Response(body=result, content_type="image/jpeg")

def setup_routes(app, handler):
    app.router.add_post('/topdf', handler.convert_to_pdf, name='topdf')
    app.router.add_post('/tojpeg', handler.convert_to_jpeg, name='tojpeg')
    app.router.add_post('/urltopdf', handler.convert_url_to_pdf, name='urltopdf')
    app.router.add_post('/urltojpeg', handler.convert_url_to_jpeg, name='urltojpeg')

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
