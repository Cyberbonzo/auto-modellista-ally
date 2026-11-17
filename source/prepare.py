"""Prepare a user-supplied PS2 disc; no game data is downloaded."""
from pathlib import Path
import hashlib,shutil,subprocess,sys,zipfile,urllib.request
ROOT=Path(__file__).resolve().parents[1]
UPSTREAM='https://github.com/exoteric33/ps2-native-recomp.git'
REVISION='15a50b77a526f779befa7addb820478485947bdc'
BOOT_SHA256='62d6380687050d425b0e3d5e58a2c54ec23909ef9ed8be816428edc7d1036c80'
def main():
    src=Path(sys.argv[1]).resolve()
    for folder in ('game/boot','source','saves','logs','docs','upstream'):(ROOT/folder).mkdir(parents=True,exist_ok=True)
    iso=ROOT/'game/disc.iso'
    if src.suffix.lower()=='.zip':
        with zipfile.ZipFile(src) as z:
            members=[x for x in z.infolist() if x.filename.lower().endswith('.iso')]
            if len(members)!=1:raise ValueError('Expected exactly one ISO in the ZIP')
            if not iso.exists():
                with z.open(members[0]) as a,iso.open('xb') as b:shutil.copyfileobj(a,b,1024*1024)
    elif src!=iso.resolve():
        if iso.exists():raise FileExistsError('game/disc.iso already exists; preserve it before replacing')
        shutil.copyfile(src,iso)
    checkout=ROOT/'upstream/ps2-native-recomp'
    if not (checkout/'.git').is_dir():
        subprocess.run(['git','clone',UPSTREAM,str(checkout)],check=True)
        subprocess.run(['git','-C',str(checkout),'checkout',REVISION],check=True)
    actual=subprocess.check_output(['git','-C',str(checkout),'rev-parse','HEAD'],text=True).strip()
    if actual!=REVISION:raise ValueError('Unexpected upstream revision; review before building')
    subprocess.run([sys.executable,str(checkout/'scripts/disc.py'),'extract',str(iso),str(ROOT/'game/boot'),'SYSTEM.CNF','SLUS_206.42'],check=True)
    elf=ROOT/'game/boot/SLUS_206.42'
    if hashlib.sha256(elf.read_bytes()).hexdigest()!=BOOT_SHA256:raise ValueError('Unsupported game executable')
    patch=urllib.request.urlopen('https://raw.githubusercontent.com/PCSX2/pcsx2_patches/efd872286d9b2ae77530b82fbc56a3110caad960/patches/SLUS-20642_6D76177B.pnach',timeout=30).read()
    # Git blob identity from the upstream patch tree, independently of HTTP headers.
    if hashlib.sha1(b'blob '+str(len(patch)).encode()+b'\0'+patch).hexdigest()!='f13d44bc6ce9f9909979104d021a15aa20283941':raise ValueError('Unexpected widescreen patch')
    (ROOT/'docs/widescreen.pnach').write_bytes(patch)
    print('Supported USA disc prepared. Run bash source/build-linux.sh on Linux.')
if __name__=='__main__':main()
