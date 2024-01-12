#!/bin/bash

make -j

echo "4 channel decoding..."
./qoiconv_orig images/screenshot_web/phoboslab.org.png orig.qoi
./qoiconv_orig orig.qoi orig.png
./qoiconv_fu orig.qoi fu.png
diff orig.png fu.png
ret=$?
if [ $ret != 0 ]; then
  ls -l orig.png fu.png
  echo "  -> qoi-fu decoder works different to original qoi implementation"
else
  echo "  -> OK"
fi

rm -f orig.qoi orig.png fu.png

echo "3 channel decoding..."
./qoiconv_orig images/photo_wikipedia/043.png orig.qoi
./qoiconv_orig orig.qoi orig.png
./qoiconv_fu orig.qoi fu.png
diff orig.png fu.png
ret=$?
if [ $ret != 0 ]; then
  ls -l orig.png fu.png
  echo "  -> qoi-fu decoder works different to original qoi implementation"
else
  echo "  -> OK"
fi

rm -f orig.qoi orig.png fu.png

echo "4 channel encoding..."
./qoiconv_orig images/screenshot_web/phoboslab.org.png orig.qoi
./qoiconv_fu images/screenshot_web/phoboslab.org.png fu.qoi
diff orig.qoi fu.qoi
ret=$?
if [ $ret != 0 ]; then
  ls -l orig.qoi fu.qoi
  echo "  -> qoi-fu encoder works different to original qoi implementation"
else
  echo "  -> OK"
fi

rm -f orig.qoi fu.qoi

echo "3 channel encoding..."
./qoiconv_orig images/photo_wikipedia/043.png orig.qoi
./qoiconv_fu images/photo_wikipedia/043.png fu.qoi
diff orig.qoi fu.qoi
ret=$?
if [ $ret != 0 ]; then
  ls -l orig.qoi fu.qoi
  echo "  -> qoi-fu encoder works different to original qoi implementation"
else
  echo "  -> OK"
fi

rm -f orig.qoi fu.qoi
