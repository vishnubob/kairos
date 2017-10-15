#!/usr/bin/env python

import glob
import cv2
import PIL
from moviepy.editor import *
import PIL.Image
import PIL.ImageOps
import PIL.ImageChops
from scipy.misc import imsave, imread, imresize
from scipy.linalg import norm
from scipy import sum, average
import numpy as np

class Image(object):
    def __init__(self, thing, mode='L', size=None, name=None):
        if type(thing) in (str, bytes):
            self.filename = thing
            self.img = PIL.Image.open(self.filename)
        else:
            self.img = thing
        self.name = name if name != None else str(id(self))
        if size != None:
            if size != self.img.size:
                self.img = self.img.resize(size)
        if self.img.mode != mode:
            self.img = self.img.convert(mode=mode)
        # if self.img.mode != "1":
        #   self.img = PIL.ImageOps.autocontrast(self.img)
        if self.img.mode == "L":
            self.img = PIL.ImageOps.equalize(self.img)
        self.size = self.img.size
        

    def __sub__(self, other):
        return PIL.ImageChops.difference(self.img, other.img)
    
    def save(self, filename):
        self.img.save(filename)

class CompareImages(object):
    SizeCache = {}

    def __init__(self, target_fn, mode="L"):
        self.mode = mode
        self.target_fn = target_fn
        self.target = Image(self.target_fn, name="target", mode=self.mode)
        self.SizeCache[self.target.size] = self.target

    def get_target(self, size):
        assert len(size) == 2
        if size not in self.SizeCache:
            img = Image(self.target_fn, size=size, mode=self.mode)
            self.SizeCache[size] = img
        return self.SizeCache[size]

    def compare_images(self, img1, img2):
        diff = img1 - img2
        diff = np.array(diff)
        # Manhattan norm
        m_norm = sum(abs(diff))
        # Zero norm
        z_norm = norm(diff.ravel(), 0)
        return (m_norm, z_norm, m_norm / diff.size, z_norm / diff.size)

    def compare(self, query_fn):
        query = Image(query_fn, name="query", mode=self.mode)
        target = self.get_target(query.size)
        return self.compare_images(query, target)

def motion(f1, f2):
    f1 = cv2.imread(f1)
    f1 = cv2.cvtColor(f1, cv2.COLOR_BGR2GRAY)
    f2 = cv2.imread(f2)
    f2 = cv2.cvtColor(f2, cv2.COLOR_BGR2GRAY)
    delta = cv2.absdiff(f1, f2)
    thresh = cv2.threshold(delta, 50, 200, cv2.THRESH_BINARY)[1]

    # dilate the thresholded image to fill in holes, then find contours
    # on thresholded image
    thresh = cv2.dilate(thresh, None, iterations=2)
    cv2.imwrite("/mnt/media/pictures/drip-shadow-motion.jpg", thresh)
    (im3, cnts, hr) = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    #res = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # loop over the contours
    min_area = 500
    for c in cnts:
        # if the contour is too small, ignore it
        if cv2.contourArea(c) < min_area:
            continue
        # compute the bounding box for the contour, draw it on the frame,
        # and update the text
        (x, y, w, h) = cv2.boundingRect(c)
        cv2.rectangle(thresh, (x, y), (x + w, y + h), (255, 255, 255), 2)
    cv2.imwrite("/mnt/media/pictures/drip-shadow-motion.jpg", thresh)

class FrameShop(object):
    def __init__(self, pics):
        self.pics = pics
        self.pics.sort()

    def compare(self, idx, shift=5):
        cp = CompareImages(self.pics[idx])
        start = max(idx - shift, 0)
        end = min(idx + shift, len(self.pics) + 1)
        for idx2 in range(start, end):
            if idx2 == idx:
                continue
            val = cp.compare(self.pics[idx2])
            yield (idx, idx2, val)


path_pics = "/mnt/media/pictures/drip-shadow/*.jpg"
pics = glob.glob(path_pics)
fs = FrameShop(pics)
import pprint
for x in range(100):
    motion(pics[x], pics[x+1])
    input("!")
