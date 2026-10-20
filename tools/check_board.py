#!/usr/bin/env python3
"""Compile the actual packaged starter example; never upload firmware."""
from pathlib import Path
import argparse,json,subprocess,sys,tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--board',choices=['uno','esp32dev'],required=True);args=p.parse_args()
 pkg=json.loads((ROOT/'library.json').read_text())['name']
 starter='CheckReadings' if pkg=='SuperJupiterSensorToolkit' else 'RecoverableRecords'
 with tempfile.TemporaryDirectory(prefix='superjupiter-packed-board-') as tmp:
  work=Path(tmp);archive=work/'package.tar.gz'
  subprocess.run([sys.executable,'-m','platformio','pkg','pack','-o',str(archive)],cwd=ROOT,check=True)
  project=work/'consumer';(project/'src').mkdir(parents=True)
  (project/'src/main.cpp').write_text('#include <'+pkg+'.h>\n'+(ROOT/'examples'/starter/(starter+'.ino')).read_text())
  platform='atmelavr@5.1.0' if args.board=='uno' else 'espressif32@6.12.0'
  (project/'platformio.ini').write_text('[env:consumer]\nplatform='+platform+'\nboard='+args.board+'\nframework=arduino\nbuild_flags=-std=gnu++11\nlib_deps=\n    file://'+archive.as_posix()+'\n')
  subprocess.run([sys.executable,'-m','platformio','run','-d',str(project)],check=True)
 print('PASS packed board compilation; hardware validation pending')
if __name__=='__main__':main()
