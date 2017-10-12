import gphoto2cffi as gp
import os
import io
import uuid

from rawkit.raw import Raw
from rawkit.options import WhiteBalance, colorspaces, gamma_curves
import PIL
from PIL import Image, ImageStat
from tempfile import mkdtemp

class CameraManager(object):
    def __iter__(self):
        return iter(gp.list_cameras())

    def __getitem__(self, idx):
        return KairosCamera(idx)

class KairosCamera(object):
    def __init__(self, idx):
        self.camera = gp.list_cameras()[idx]

    def __iter__(self):
        return iter(tuple(self.camera.list_all_files()))

    def remove_all(self):
        for image in self:
            image.remove()

    def download(self, fileidx=0):
        fh = list(self)[fileidx]
        data = fh.get_data()
        fh.remove()
        return data

def lit_pixels(data):
    try:
        img = Image.open(io.BytesIO(data))
    except:
        img = develop(data)
        img = Image.open(img)
    img = img.convert('L')
    stats = ImageStat.Stat(img)
    return (stats.mean[0], stats.stddev[0])

def develop(data, ext="png"):
    ext = (s for s in ext.split('.') if len(s))
    ext = str(*ext)
    tempdir = mkdtemp()
    stemfn = str(uuid.uuid4())
    stemfn = os.path.join(tempdir, stemfn)
    rawfn = stemfn + ".raw"
    intfn = stemfn + ".tiff"
    with open(rawfn, 'wb') as fh:
        fh.write(data)

    # raw to tiff
    raw = Raw(rawfn)
    raw.options.white_balance = WhiteBalance(camera=False, auto=True)
    raw.options.colorspace = colorspaces.adobe_rgb
    raw.options.gamma = gamma_curves.adobe_rgb
    raw.save(filename=intfn)

    # tiff to final format
    img = Image.open(intfn)
    new_img = io.BytesIO()
    img.save(new_img, format=ext)
    return new_img.getvalue()
