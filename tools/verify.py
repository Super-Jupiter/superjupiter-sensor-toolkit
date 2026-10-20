#!/usr/bin/env python3
"""Offline manifest/provenance checks and optional CMake regression execution."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
 p=argparse.ArgumentParser();p.add_argument('--test',action='store_true');p.add_argument('--compiler');args=p.parse_args()
 manifest=json.loads((ROOT/'library.json').read_text());base=json.loads((ROOT/'BASELINE.json').read_text())
 props=dict(line.split('=',1) for line in (ROOT/'library.properties').read_text().splitlines() if '=' in line)
 assert manifest['name']==props['name']==base['bundle']
 assert manifest['version']==props['version']==base['version']=='0.1.0'
 assert manifest['license']==props['license']=='MIT'
 assert 'hardware validation pending' in (ROOT/'README.md').read_text().lower()
 assert not manifest.get('dependencies') and not props.get('depends')
 assert 'VERSION '+base['version']+' LANGUAGES' in (ROOT/'CMakeLists.txt').read_text()
 for c in base['components']:
  header=(ROOT/'src'/c['newHeader']).read_bytes()
  assert hashlib.sha256(header).hexdigest()==c['consolidatedHeaderSha256']
  prefix='// Consolidated public release baseline: '+c['package']+' '+c['version']+'.\n#if defined('+c['originalGuard']+')\n#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"\n#endif\n'
  text=header.decode();assert text.startswith(prefix)
  original=text[len(prefix):].replace(c['consolidatedGuard'],c['originalGuard'])
  assert hashlib.sha256(original.encode()).hexdigest()==c['originalHeaderSha256'],'Algorithm body changed'
  assert re.fullmatch('[0-9a-f]{40}',c['publicCommit'])
  assert c['publicUrl'].startswith('https://codeload.github.com/Super-Jupiter/superjupiter-')
  assert not (ROOT/'src'/(c['package']+'.h')).exists(),'Legacy flat header must not be exported'
 for f in ROOT.rglob('*'):
  if not f.is_file() or any(part in ('build','.pio','.git') for part in f.relative_to(ROOT).parts):continue
  assert f.suffix not in ('.pem','.key','.p12','.sqlite','.db')
  if f.suffix in ('.h','.cpp','.ino','.json','.md','.py','.txt','.in','.properties'):
   data=f.read_text(encoding='utf-8')
   assert ('C:'+'/Users/') not in data and ('/mnt/c/'+'Users/') not in data,'Machine-local path in package'
 if args.test:
  with tempfile.TemporaryDirectory(prefix='superjupiter-bundle-check-') as tmp:
   cmd=['cmake','-S',str(ROOT),'-B',tmp]
   if args.compiler:cmd+=['-DCMAKE_CXX_COMPILER='+args.compiler]
   subprocess.run(cmd,check=True);subprocess.run(['cmake','--build',tmp,'-j4'],check=True)
   subprocess.run(['ctest','--test-dir',tmp,'--output-on-failure'],check=True)
 print('PASS:',manifest['name'],manifest['version'],'metadata and exact public algorithm provenance; hardware validation pending')
if __name__=='__main__':main()
