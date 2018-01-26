
%global debug_package %{nil}

%define name Ultimarc-linux
%define version 1.1.0

Name: %{name}
Version: %{version}
Release: 1
Summary: Ultimarc-linux library and command line utility

License: Copyright (C) 2014 - Robert Abram, Katie Snow

URL: https://github.com/katie-snow/Ultimarc-linux
Source0: %{name}.tar.xz

BuildRequires: autoconf automake libtool gcc libusb-devel json-c-devel
Requires: libusb json-c

BuildArch: x86_64

%description
Provides Ultimarc-linux library and command line utility for programming Ultimarc devices

%prep
%setup -q -n Ultimarc-linux

libtoolize
aclocal
./autogen.sh

%build
make clean
make

%install
# executable
install -d %{buildroot}/%{_bindir}
install -p -m 755 ./src/umtool/umtool %{buildroot}/%{_bindir}/

# udev rule
install -d %{buildroot}/usr/lib/udev/rules.d
install -p 21-ultimarc.rules %{buildroot}/usr/lib/udev/rules.d/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/umtool
/usr/lib/udev/rules.d/21-ultimarc.rules

%post
udevadm control --reload
