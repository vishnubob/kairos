import gphoto2cffi as gp
import io
import uuid

from rawkit.raw import Raw
from rawkit.options import WhiteBalance, colorspaces, gamma_curves
import PIL
from PIL import Image
from tempfile import mkdtemp

class CameraManager(object):
    def __iter__(self):
        return iter(gp.list_cameras())

    def __getitem__(self, idx):
        return KairosCamera(idx)

class KairosCamera(gp.Camera):
    def __init__(self, idx):
        cam = gp.list_cameras()[idx]
        (bud, device) = cam._usb_address
        super(KairosCamera, self).__init__(bus=bus, device=device)

        # Get a list of files on the camera
        files = tuple(my_cam.list_all_files())

    def download(self, fileidx=0, filehandle=None):
        if filename == None:
            filehandle = io.StringIO()
        else:
            filehandle = open(filename, 'wb')
        for chunk in my_cam.files[fileidx].iter_data():
            filehandle.write(chunk)
        return filehandle

def lit_pixels(data):
    try:
        img = Image.open(data)
    except:
        img = develop(data)
        img = Image.open(data)
    img = img.convert('1')
    stats = PIL.ImageStat.Stat(img)
    return stats.sum / float(stats.count)

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
