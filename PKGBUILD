# Maintainer: Brandon Surmanski <b.surmanski@gmail.com>

pkgname=ncxbacklight-git
pkgver=v0.1.r1.g12ba77c
pkgrel=1
epoch=
pkgdesc="NCurses Interface for XBacklight"
arch=(any)
url="www.github.com/bsurmanski/ncxbacklight"
license=('MIT')
groups=()
depends=("xcb-util" "ncurses")
makedepends=("git")
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=(ncxbacklight-git::git+https://github.com/bsurmanski/ncxbacklight)
noextract=()
md5sum=('SKIP')
sha256sums=('SKIP')

pkgver() {
    cd "$pkgname"
    git describe --long | sed -r 's/([^-]*-g)/r\1/;s/-/./g'
}

build() {
    cd "$pkgname"
    make
}

check() {
    cd "$pkgname"
    make -k check
}

package() {
    cd "$srcdir/$pkgname-pkgver"
    install -Dm644 "$pkgdir/usr/bin/"
}

#vim:set ts=2 sw=2 et:
