#!/usr/bin/python
#
# Copyright (c) 2016 Intel, Inc. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

import os

DEFAULT_FILENAME="/tmp/libudplugin_sample.so"

def write_binary_file(filename=DEFAULT_FILENAME):
    try:
        f = open(filename, "wb")
        f.write(UDSENSOR_SAMPLE.decode('base64'))
        f.close()
    except Exception as e:
        print(e)

def delete_binary_file(filename=DEFAULT_FILENAME):
    try:
        os.remove(filename)
    except Exception as e:
        print(e)

#
# The following is a base64 enconded sensor sample for unit testing.
# The purpose is to write this binary into a known location and load
# it from the tests.
#
# This string was generated with :
# $ cat your_shared.so | base64 > somefile.txt
#

UDSENSOR_SAMPLE = b"""
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
aXN0ZXJDbGFzc2VzAF9aTjEzZ2VtZXRlclNlbnNvckQyRXYAX1pUVjEzZ2VtZXRlclNlbnNvcgBf
Wk4xM2dlbWV0ZXJTZW5zb3JEMUV2AF9aTjEzZ2VtZXRlclNlbnNvcjRpbml0RXYAX1pOMTNnZW1l
dGVyU2Vuc29yOGZpbmFsaXplRXYAX1pOMTNnZW1ldGVyU2Vuc29yM2xvZ0V2AF9aTjhteVNlbnNv
ckQyRXYAX1pOOG15U2Vuc29yRDFFdgBfWk4xM2dlbWV0ZXJTZW5zb3JEMEV2AF9aZGxQdgBfWk44
bXlTZW5zb3JEMEV2AF9aTlN0OWJhc2ljX2lvc0ljU3QxMWNoYXJfdHJhaXRzSWNFRTVjbGVhckVT
dDEyX0lvc19Jb3N0YXRlAF9aTjhteVNlbnNvcjRpbml0RXYAX1pTdDRjb3V0AF9aU3QxNl9fb3N0
cmVhbV9pbnNlcnRJY1N0MTFjaGFyX3RyYWl0c0ljRUVSU3QxM2Jhc2ljX29zdHJlYW1JVF9UMF9F
UzZfUEtTM19sAF9aTjhteVNlbnNvcjNsb2dFdgBfWk44bXlTZW5zb3I4ZmluYWxpemVFdgBpbml0
UGx1Z2luAF9abndtAF9aVFY4bXlTZW5zb3IAX1pOU3Q4aW9zX2Jhc2U0SW5pdEMxRXYAX1pOU3Q4
aW9zX2Jhc2U0SW5pdEQxRXYAX19jeGFfYXRleGl0AF9aVEkxM2dlbWV0ZXJTZW5zb3IAX1pUVk4x
MF9fY3h4YWJpdjExN19fY2xhc3NfdHlwZV9pbmZvRQBfWlRTMTNnZW1ldGVyU2Vuc29yAF9aVFM4
bXlTZW5zb3IAX1pUSThteVNlbnNvcgBfWlRWTjEwX19jeHhhYml2MTIwX19zaV9jbGFzc190eXBl
X2luZm9FAHBsdWdpbk5hbWUAbGlic3RkYysrLnNvLjYAbGlibS5zby42AGxpYmdjY19zLnNvLjEA
bGliYy5zby42AF9lZGF0YQBfX2Jzc19zdGFydABfZW5kAG15c2Vuc29yLnNvAEdMSUJDXzIuMi41
AEdMSUJDWFhfMy40LjkAQ1hYQUJJXzEuMwBHTElCQ1hYXzMuNAAAAAAAAAAAAAIAAgADAAIAAAAA
AAMABAACAAUABAACAAIAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEA
AQABAAEAAQABAAEATQMAABAAAAAgAAAAdRppCQAAAwB7AwAAAAAAAAEAAwAmAwAAEAAAAAAAAACJ
fykCAAAFAIcDAAAQAAAA069rBQAABACVAwAAEAAAAHQpkggAAAIAoAMAAAAAAADYHCAAAAAAAAgA
AAAAAAAAcBAAAAAAAADgHCAAAAAAAAgAAAAAAAAAkA8AAAAAAADoHCAAAAAAAAgAAAAAAAAAMBAA
AAAAAAAAHSAAAAAAAAgAAAAAAAAAAB0gAAAAAABYICAAAAAAAAgAAAAAAAAA7hEAAAAAAAAQHSAA
AAAAAAEAAAALAAAAEAAAAAAAAAAYHSAAAAAAAAEAAAAgAAAAAAAAAAAAAAAgHSAAAAAAAAEAAAAO
AAAAEAAAAAAAAAAoHSAAAAAAAAEAAAAkAAAAAAAAAAAAAAAwHSAAAAAAAAEAAAAYAAAAAAAAAAAA
AABIHSAAAAAAAAEAAAAYAAAAAAAAAAAAAABQHSAAAAAAAAEAAAARAAAAAAAAAAAAAABYHSAAAAAA
AAEAAAApAAAAAAAAAAAAAABgHSAAAAAAAAEAAAAcAAAAAAAAAAAAAABoHSAAAAAAAAEAAAATAAAA
AAAAAAAAAABwHSAAAAAAAAEAAAAbAAAAAAAAAAAAAACIHSAAAAAAAAEAAAAmAAAAAAAAAAAAAACQ
HSAAAAAAAAEAAAAlAAAAAAAAAAAAAACYHSAAAAAAAAEAAAAjAAAAAAAAAAAAAACgHSAAAAAAAAEA
AAAeAAAAAAAAAAAAAACoHSAAAAAAAAEAAAAiAAAAAAAAAAAAAACwHSAAAAAAAAEAAAAnAAAAAAAA
AAAAAAC4HyAAAAAAAAYAAAACAAAAAAAAAAAAAADAHyAAAAAAAAYAAAADAAAAAAAAAAAAAADIHyAA
AAAAAAYAAAAHAAAAAAAAAAAAAADQHyAAAAAAAAYAAAAIAAAAAAAAAAAAAADYHyAAAAAAAAYAAAAJ
AAAAAAAAAAAAAADgHyAAAAAAAAYAAAAKAAAAAAAAAAAAAADoHyAAAAAAAAYAAAAMAAAAAAAAAAAA
AADwHyAAAAAAAAYAAAAXAAAAAAAAAAAAAAD4HyAAAAAAAAYAAAAhAAAAAAAAAAAAAAAYICAAAAAA
AAcAAAACAAAAAAAAAAAAAAAgICAAAAAAAAcAAAAEAAAAAAAAAAAAAAAoICAAAAAAAAcAAAAFAAAA
AAAAAAAAAAAwICAAAAAAAAcAAAAGAAAAAAAAAAAAAAA4ICAAAAAAAAcAAAAKAAAAAAAAAAAAAABA
ICAAAAAAAAcAAAANAAAAAAAAAAAAAABIICAAAAAAAAcAAAAPAAAAAAAAAAAAAABQICAAAAAAAAcA
AAAQAAAAAAAAAAAAAABIg+wISIsF7RAgAEiFwHQF6BsAAABIg8QIwwAAAAAAAP81IhEgAP8lJBEg
AA8fQAD/JSIRIABoAAAAAOng/////yUaESAAaAEAAADp0P////8lEhEgAGgCAAAA6cD/////JQoR
IABoAwAAAOmw/////yUCESAAaAQAAADpoP////8l+hAgAGgFAAAA6ZD/////JfIQIABoBgAAAOmA
/////yXqECAAaAcAAADpcP///1BIiwdIA3joi3cgg84B6M3///9aw2YuDx+EAAAAAACQSI09zRAg
AEiD7AjocP///0iLPSEQIABIjRVSDSAASI01rxAgAEiDxAjpYv///2aQSI0FoBAgAEiNPZIQIABV
SCn4SInlSIP4DncCXcNIiwXsDyAASIXAdPJd/+APH0AASI0FaRAgAEiNPWIQIABVSCn4SInlSMH4
A0iJwkjB6j9IAdBI0fh1Al3DSIsVtw8gAEiF0nTyXUiJxv/iDx9AAIA9KRAgAAB1J0iDPZ8PIAAA
VUiJ5XQMSI09sgwgAOjd/v//6Gj///9dxgUAECAAAfPDZmZmZmYuDx+EAAAAAABIgz14DCAAAHQm
SIsFPw8gAEiFwHQaVUiNPWIMIABIieX/0F3pV////w8fgAAAAADpS////2YuDx+EAAAAAACQSIs9
MQ8gAEiNNRIBAAC6CAAAAOl4/v//Dx+EAAAAAABIiz0RDyAASI01+wAAALoHAAAA6Vj+//8PH4QA
AAAAAEiLPfEOIABIjTXjAAAAugwAAADpOP7//w8fhAAAAAAASIPsCL8IAAAA6EL+//9IixXTDiAA
SIPCEEiJEEiDxAjDZi4PH4QAAAAAAA8fRAAASIsFqQ4gAEiDwBBIiQfDkPPDZi4PH4QAAAAAAA8f
QADzw2YuDx+EAAAAAAAPH0AA88NmLg8fhAAAAAAADx9AAEiLBWkOIABIg8AQSIkHw5BIiwVZDiAA
SIPAEEiJB+ld/f//Zi4PH4QAAAAAAA8fAEiLBTkOIABIg8AQSIkH6T39//+QSIPsCEiDxAjDAAAA
T24gaW5pdAoAT24gbG9nCgBPbiBmaW5hbGl6ZQoATXlTZW5zb3IAAAAAAAAAAAAAMTNnZW1ldGVy
U2Vuc29yADhteVNlbnNvcgAAAAEbAzt4AAAADgAAAMT8//+UAAAAVP3//2QBAAB0/f//3AEAAJT+
//98AQAAtP7//5QBAADU/v//rAEAAPT+///EAQAAJP///7wAAAA0////1AAAAET////sAAAAVP//
/wQBAABk////HAEAAHT///80AQAAlP///0wBAAAUAAAAAAAAAAF6UgABeBABGwwHCJABAAAkAAAA
HAAAACj8//+QAAAAAA4QRg4YSg8LdwiAAD8aOyozJCIAAAAAFAAAAEQAAABg/v//DwAAAAAAAAAA
AAAAFAAAAFwAAABY/v//AgAAAAAAAAAAAAAAFAAAAHQAAABQ/v//AgAAAAAAAAAAAAAAFAAAAIwA
AABI/v//AgAAAAAAAAAAAAAAFAAAAKQAAABA/v//DwAAAAAAAAAAAAAAFAAAALwAAAA4/v//EwAA
AAAAAAAAAAAAFAAAANQAAABA/v//EwAAAAAAAAAAAAAAFAAAAOwAAADo+///FQAAAABBDhBTDggA
FAAAAAQBAAAQ/f//GAAAAAAAAAAAAAAAFAAAABwBAAAY/f//GAAAAAAAAAAAAAAAFAAAADQBAAAg
/f//GAAAAAAAAAAAAAAAFAAAAEwBAAAo/f//IQAAAABEDhBcDggAFAAAAGQBAACQ+///LgAAAABL
DhBeDggAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAHAQAAAAAAAAkA8AAAAAAAAwEAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAHSAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAmAwAAAAAAAAEAAAAAAAAANQMAAAAA
AAABAAAAAAAAAD8DAAAAAAAAAQAAAAAAAABNAwAAAAAAAA4AAAAAAAAAbwMAAAAAAAAMAAAAAAAA
AMAOAAAAAAAADQAAAAAAAADEEQAAAAAAABkAAAAAAAAA2BwgAAAAAAAbAAAAAAAAABAAAAAAAAAA
GgAAAAAAAADoHCAAAAAAABwAAAAAAAAACAAAAAAAAAD1/v9vAAAAAPABAAAAAAAABQAAAAAAAAC4
BgAAAAAAAAYAAAAAAAAAyAIAAAAAAAAKAAAAAAAAAKwDAAAAAAAACwAAAAAAAAAYAAAAAAAAAAMA
AAAAAAAAACAgAAAAAAACAAAAAAAAAMAAAAAAAAAAFAAAAAAAAAAHAAAAAAAAABcAAAAAAAAAAA4A
AAAAAAAHAAAAAAAAABgLAAAAAAAACAAAAAAAAADoAgAAAAAAAAkAAAAAAAAAGAAAAAAAAAD+//9v
AAAAALgKAAAAAAAA////bwAAAAACAAAAAAAAAPD//28AAAAAZAoAAAAAAAD5//9vAAAAAAUAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC4HSAAAAAAAAAAAAAAAAAA
AAAAAAAAAAD2DgAAAAAAAAYPAAAAAAAAFg8AAAAAAAAmDwAAAAAAADYPAAAAAAAARg8AAAAAAABW
DwAAAAAAAGYPAAAAAAAA7hEAAAAAAABHQ0M6IChHTlUpIDQuOC41IDIwMTUwNjIzIChSZWQgSGF0
IDQuOC41LTQpAAAuc3ltdGFiAC5zdHJ0YWIALnNoc3RydGFiAC5ub3RlLmdudS5idWlsZC1pZAAu
Z251Lmhhc2gALmR5bnN5bQAuZHluc3RyAC5nbnUudmVyc2lvbgAuZ251LnZlcnNpb25fcgAucmVs
YS5keW4ALnJlbGEucGx0AC5pbml0AC50ZXh0AC5maW5pAC5yb2RhdGEALmVoX2ZyYW1lX2hkcgAu
ZWhfZnJhbWUALmluaXRfYXJyYXkALmZpbmlfYXJyYXkALmpjcgAuZGF0YS5yZWwucm8ALmR5bmFt
aWMALmdvdAAuZ290LnBsdAAuZGF0YQAuYnNzAC5jb21tZW50AAAAAAAAAAAAAAAAAAAAAAAAAAAA
AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABsAAAAHAAAA
AgAAAAAAAADIAQAAAAAAAMgBAAAAAAAAJAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAu
AAAA9v//bwIAAAAAAAAA8AEAAAAAAADwAQAAAAAAANgAAAAAAAAAAwAAAAAAAAAIAAAAAAAAAAAA
AAAAAAAAOAAAAAsAAAACAAAAAAAAAMgCAAAAAAAAyAIAAAAAAADwAwAAAAAAAAQAAAACAAAACAAA
AAAAAAAYAAAAAAAAAEAAAAADAAAAAgAAAAAAAAC4BgAAAAAAALgGAAAAAAAArAMAAAAAAAAAAAAA
AAAAAAEAAAAAAAAAAAAAAAAAAABIAAAA////bwIAAAAAAAAAZAoAAAAAAABkCgAAAAAAAFQAAAAA
AAAAAwAAAAAAAAACAAAAAAAAAAIAAAAAAAAAVQAAAP7//28CAAAAAAAAALgKAAAAAAAAuAoAAAAA
AABgAAAAAAAAAAQAAAACAAAACAAAAAAAAAAAAAAAAAAAAGQAAAAEAAAAAgAAAAAAAAAYCwAAAAAA
ABgLAAAAAAAA6AIAAAAAAAADAAAAAAAAAAgAAAAAAAAAGAAAAAAAAABuAAAABAAAAAIAAAAAAAAA
AA4AAAAAAAAADgAAAAAAAMAAAAAAAAAAAwAAAAoAAAAIAAAAAAAAABgAAAAAAAAAeAAAAAEAAAAG
AAAAAAAAAMAOAAAAAAAAwA4AAAAAAAAaAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAHMA
AAABAAAABgAAAAAAAADgDgAAAAAAAOAOAAAAAAAAkAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAEAAA
AAAAAAB+AAAAAQAAAAYAAAAAAAAAcA8AAAAAAABwDwAAAAAAAFQCAAAAAAAAAAAAAAAAAAAQAAAA
AAAAAAAAAAAAAAAAhAAAAAEAAAAGAAAAAAAAAMQRAAAAAAAAxBEAAAAAAAAJAAAAAAAAAAAAAAAA
AAAABAAAAAAAAAAAAAAAAAAAAIoAAAABAAAAAgAAAAAAAADQEQAAAAAAANARAAAAAAAASgAAAAAA
AAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAACSAAAAAQAAAAIAAAAAAAAAHBIAAAAAAAAcEgAAAAAA
AHwAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAoAAAAAEAAAACAAAAAAAAAJgSAAAAAAAA
mBIAAAAAAAB8AQAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAKoAAAAOAAAAAwAAAAAAAADY
HCAAAAAAANgcAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAC2AAAADwAAAAMA
AAAAAAAA6BwgAAAAAADoHAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAwgAA
AAEAAAADAAAAAAAAAPAcIAAAAAAA8BwAAAAAAAAIAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAA
AAAAAMcAAAABAAAAAwAAAAAAAAAAHSAAAAAAAAAdAAAAAAAAuAAAAAAAAAAAAAAAAAAAACAAAAAA
AAAAAAAAAAAAAADUAAAABgAAAAMAAAAAAAAAuB0gAAAAAAC4HQAAAAAAAAACAAAAAAAABAAAAAAA
AAAIAAAAAAAAABAAAAAAAAAA3QAAAAEAAAADAAAAAAAAALgfIAAAAAAAuB8AAAAAAABIAAAAAAAA
AAAAAAAAAAAACAAAAAAAAAAIAAAAAAAAAOIAAAABAAAAAwAAAAAAAAAAICAAAAAAAAAgAAAAAAAA
WAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAACAAAAAAAAADrAAAAAQAAAAMAAAAAAAAAWCAgAAAAAABY
IAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAA8QAAAAgAAAADAAAAAAAAAGAg
IAAAAAAAYCAAAAAAAAAIAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAPYAAAABAAAAMAAA
AAAAAAAAAAAAAAAAAGAgAAAAAAAALAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAQAAAAAAAAARAAAA
AwAAAAAAAAAAAAAAAAAAAAAAAACMIAAAAAAAAP8AAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAA
AAAAAQAAAAIAAAAAAAAAAAAAAAAAAAAAAAAA0CgAAAAAAAAoCAAAAAAAABwAAAAvAAAACAAAAAAA
AAAYAAAAAAAAAAkAAAADAAAAAAAAAAAAAAAAAAAAAAAAAPgwAAAAAAAATQUAAAAAAAAAAAAAAAAA
AAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwABAMgBAAAAAAAA
AAAAAAAAAAAAAAAAAwACAPABAAAAAAAAAAAAAAAAAAAAAAAAAwADAMgCAAAAAAAAAAAAAAAAAAAA
AAAAAwAEALgGAAAAAAAAAAAAAAAAAAAAAAAAAwAFAGQKAAAAAAAAAAAAAAAAAAAAAAAAAwAGALgK
AAAAAAAAAAAAAAAAAAAAAAAAAwAHABgLAAAAAAAAAAAAAAAAAAAAAAAAAwAIAAAOAAAAAAAAAAAA
AAAAAAAAAAAAAwAJAMAOAAAAAAAAAAAAAAAAAAAAAAAAAwAKAOAOAAAAAAAAAAAAAAAAAAAAAAAA
AwALAHAPAAAAAAAAAAAAAAAAAAAAAAAAAwAMAMQRAAAAAAAAAAAAAAAAAAAAAAAAAwANANARAAAA
AAAAAAAAAAAAAAAAAAAAAwAOABwSAAAAAAAAAAAAAAAAAAAAAAAAAwAPAJgSAAAAAAAAAAAAAAAA
AAAAAAAAAwAQANgcIAAAAAAAAAAAAAAAAAAAAAAAAwARAOgcIAAAAAAAAAAAAAAAAAAAAAAAAwAS
APAcIAAAAAAAAAAAAAAAAAAAAAAAAwATAAAdIAAAAAAAAAAAAAAAAAAAAAAAAwAUALgdIAAAAAAA
AAAAAAAAAAAAAAAAAwAVALgfIAAAAAAAAAAAAAAAAAAAAAAAAwAWAAAgIAAAAAAAAAAAAAAAAAAA
AAAAAwAXAFggIAAAAAAAAAAAAAAAAAAAAAAAAwAYAGAgIAAAAAAAAAAAAAAAAAAAAAAAAwAZAAAA
AAAAAAAAAAAAAAAAAAABAAAABADx/wAAAAAAAAAAAAAAAAAAAAAOAAAAAgALAHAPAAAAAAAAFQAA
AAAAAABNAAAAAgALAJAPAAAAAAAALgAAAAAAAABpAAAAAQAYAGQgIAAAAAAAAQAAAAAAAAB4AAAA
BADx/wAAAAAAAAAAAAAAAAAAAACDAAAAAQASAPAcIAAAAAAAAAAAAAAAAACQAAAAAgALAMAPAAAA
AAAAAAAAAAAAAAClAAAAAgALAPAPAAAAAAAAAAAAAAAAAAC4AAAAAgALADAQAAAAAAAAAAAAAAAA
AADOAAAAAQAYAGAgIAAAAAAAAQAAAAAAAADdAAAAAQARAOgcIAAAAAAAAAAAAAAAAAAEAQAAAgAL
AHAQAAAAAAAAAAAAAAAAAAAQAQAAAQAQANgcIAAAAAAAAAAAAAAAAAB4AAAABADx/wAAAAAAAAAA
AAAAAAAAAAAvAQAAAQAPABAUAAAAAAAAAAAAAAAAAAA9AQAAAQASAPAcIAAAAAAAAAAAAAAAAAAA
AAAABADx/wAAAAAAAAAAAAAAAAAAAABJAQAAAQAWAAAgIAAAAAAAAAAAAAAAAABfAQAAAQAXAGAg
IAAAAAAAAAAAAAAAAABrAQAAAQATAAAdIAAAAAAAAAAAAAAAAAB4AQAAAQAUALgdIAAAAAAAAAAA
AAAAAACBAQAAEgALANAQAAAAAAAAGAAAAAAAAACUAQAAIAAAAAAAAAAAAAAAAAAAAAAAAACjAQAA
IAAAAAAAAAAAAAAAAAAAAAAAAAC3AQAAEgAAAAAAAAAAAAAAAAAAAAAAAADLAQAAEgAMAMQRAAAA
AAAAAAAAAAAAAADRAQAAEgAAAAAAAAAAAAAAAAAAAAAAAAD2AQAAEQAXAFggIAAAAAAACAAAAAAA
AAABAgAAIgALAGARAAAAAAAAAgAAAAAAAAAfAgAAEgAAAAAAAAAAAAAAAAAAAAAAAAA5AgAAEgAA
AAAAAAAAAAAAAAAAAAAAAABeAgAAIAAAAAAAAAAAAAAAAAAAAAAAAAB6AgAAIAAAAAAAAAAAAAAA
AAAAAAAAAACUAgAAIgALAHARAAAAAAAAAgAAAAAAAACtAgAAIgALAEARAAAAAAAADwAAAAAAAADE
AgAAEgALAPAQAAAAAAAAGAAAAAAAAADcAgAAIgAAAAAAAAAAAAAAAAAAAAAAAAD4AgAAEQAAAAAA
AAAAAAAAAAAAAAAAAAAqAwAAIgALAFARAAAAAAAAAgAAAAAAAABEAwAAEQAAAAAAAAAAAAAAAAAA
AAAAAABbAwAAEgALALAQAAAAAAAAGAAAAAAAAABvAwAAIgALALARAAAAAAAAEwAAAAAAAACAAwAA
EgAAAAAAAAAAAAAAAAAAAAAAAADdAwAAIgALAJARAAAAAAAAEwAAAAAAAAD0AwAAEAAYAGAgIAAA
AAAAAAAAAAAAAAAABAAAEQAAAAAAAAAAAAAAAAAAAAAAAAA1BAAAIQATAEAdIAAAAAAAOAAAAAAA
AABJBAAAIQANABASAAAAAAAACgAAAAAAAABXBAAAIQATABAdIAAAAAAAEAAAAAAAAABrBAAAIgAL
AIARAAAAAAAADwAAAAAAAAB8BAAAIgALAEARAAAAAAAADwAAAAAAAACTBAAAEgAAAAAAAAAAAAAA
AAAAAAAAAADcBAAAEAAYAGggIAAAAAAAAAAAAAAAAADhBAAAIgALAIARAAAAAAAADwAAAAAAAADy
BAAAIQATACAdIAAAAAAAGAAAAAAAAAAABQAAIQATAIAdIAAAAAAAOAAAAAAAAAAOBQAAEAAXAGAg
IAAAAAAAAAAAAAAAAAAVBQAAEgAAAAAAAAAAAAAAAAAAAAAAAAAoBQAAIQANAAASAAAAAAAAEAAA
AAAAAAA8BQAAEgALABARAAAAAAAAIQAAAAAAAABHBQAAEgAJAMAOAAAAAAAAAAAAAAAAAAAAbXlz
ZW5zb3IuY3BwAF9aU3Rsc0lTdDExY2hhcl90cmFpdHNJY0VFUlN0MTNiYXNpY19vc3RyZWFtSWNU
X0VTNV9QS2MucGFydC4xAF9HTE9CQUxfX3N1Yl9JX215c2Vuc29yLmNwcABfWlN0TDhfX2lvaW5p
dABjcnRzdHVmZi5jAF9fSkNSX0xJU1RfXwBkZXJlZ2lzdGVyX3RtX2Nsb25lcwByZWdpc3Rlcl90
bV9jbG9uZXMAX19kb19nbG9iYWxfZHRvcnNfYXV4AGNvbXBsZXRlZC42MzQyAF9fZG9fZ2xvYmFs
X2R0b3JzX2F1eF9maW5pX2FycmF5X2VudHJ5AGZyYW1lX2R1bW15AF9fZnJhbWVfZHVtbXlfaW5p
dF9hcnJheV9lbnRyeQBfX0ZSQU1FX0VORF9fAF9fSkNSX0VORF9fAF9HTE9CQUxfT0ZGU0VUX1RB
QkxFXwBfX1RNQ19FTkRfXwBfX2Rzb19oYW5kbGUAX0RZTkFNSUMAX1pOOG15U2Vuc29yM2xvZ0V2
AF9fZ21vbl9zdGFydF9fAF9Kdl9SZWdpc3RlckNsYXNzZXMAX1pkbFB2QEBHTElCQ1hYXzMuNABf
ZmluaQBfWk5TdDhpb3NfYmFzZTRJbml0QzFFdkBAR0xJQkNYWF8zLjQAcGx1Z2luTmFtZQBfWk4x
M2dlbWV0ZXJTZW5zb3I4ZmluYWxpemVFdgBfX2N4YV9hdGV4aXRAQEdMSUJDXzIuMi41AF9aTlN0
OGlvc19iYXNlNEluaXREMUV2QEBHTElCQ1hYXzMuNABfSVRNX2RlcmVnaXN0ZXJUTUNsb25lVGFi
bGUAX0lUTV9yZWdpc3RlclRNQ2xvbmVUYWJsZQBfWk4xM2dlbWV0ZXJTZW5zb3IzbG9nRXYAX1pO
MTNnZW1ldGVyU2Vuc29yRDFFdgBfWk44bXlTZW5zb3I4ZmluYWxpemVFdgBfX2N4YV9maW5hbGl6
ZUBAR0xJQkNfMi4yLjUAX1pUVk4xMF9fY3h4YWJpdjExN19fY2xhc3NfdHlwZV9pbmZvRUBAQ1hY
QUJJXzEuMwBfWk4xM2dlbWV0ZXJTZW5zb3I0aW5pdEV2AF9aU3Q0Y291dEBAR0xJQkNYWF8zLjQA
X1pOOG15U2Vuc29yNGluaXRFdgBfWk44bXlTZW5zb3JEMEV2AF9aU3QxNl9fb3N0cmVhbV9pbnNl
cnRJY1N0MTFjaGFyX3RyYWl0c0ljRUVSU3QxM2Jhc2ljX29zdHJlYW1JVF9UMF9FUzZfUEtTM19s
QEBHTElCQ1hYXzMuNC45AF9aTjEzZ2VtZXRlclNlbnNvckQwRXYAX19ic3Nfc3RhcnQAX1pUVk4x
MF9fY3h4YWJpdjEyMF9fc2lfY2xhc3NfdHlwZV9pbmZvRUBAQ1hYQUJJXzEuMwBfWlRWMTNnZW1l
dGVyU2Vuc29yAF9aVFM4bXlTZW5zb3IAX1pUSTEzZ2VtZXRlclNlbnNvcgBfWk44bXlTZW5zb3JE
MUV2AF9aTjEzZ2VtZXRlclNlbnNvckQyRXYAX1pOU3Q5YmFzaWNfaW9zSWNTdDExY2hhcl90cmFp
dHNJY0VFNWNsZWFyRVN0MTJfSW9zX0lvc3RhdGVAQEdMSUJDWFhfMy40AF9lbmQAX1pOOG15U2Vu
c29yRDJFdgBfWlRJOG15U2Vuc29yAF9aVFY4bXlTZW5zb3IAX2VkYXRhAF9abndtQEBHTElCQ1hY
XzMuNABfWlRTMTNnZW1ldGVyU2Vuc29yAGluaXRQbHVnaW4AX2luaXQA
"""