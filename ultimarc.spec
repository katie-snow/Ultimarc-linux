
%global debug_package %{nil}

%define name ultimarc
%define version 1.1.0

Name: %{name}
Version: %{version}
Release: 12
Summary: Ultimarc-linux library and command line utility

License: Copyright (C) 2014 - Robert Abram, Katie Snow

URL: https://github.com/katie-snow/Ultimarc-linux
Source0: Ultimarc-linux.tar.xz

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
# static library
install -d %{buildroot}/%{_libdir}/ultimarc
install -p -m 644 ./src/libs/libultimarc.la %{buildroot}/%{_libdir}/ultimarc/

# header file
install -d %{buildroot}/%{_includedir}/ultimarc
install -p -m 644 ./src/libs/ultimarc.h %{buildroot}/%{_includedir}/ultimarc/

# executable
install -d %{buildroot}/%{_bindir}
install -p -m 755 ./src/umtool/umtool %{buildroot}/%{_bindir}/

# udev rule
install -d %{buildroot}/usr/lib/udev/rules.d
install -p 21-ultimarc.rules %{buildroot}/usr/lib/udev/rules.d/

#example json files
install -d %{buildroot}/%{_datarootdir}/ultimarc/examples
install -p -m 644 ./README %{buildroot}/%{_datarootdir}/ultimarc/
install -p -m 644 ./src/umtool/*.json %{buildroot}/%{_datarootdir}/ultimarc/examples/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/umtool
/usr/lib/udev/rules.d/21-ultimarc.rules
%{_datarootdir}/ultimarc

%post
udevadm control --reload


%package devel
Requires: ultimarc
Summary: Ultimarc development header file and static library file

%description devel
Ultimarc development header file and static library file

%files devel
%{_libdir}/ultimarc
%{_includedir}/ultimarc
