
%global debug_package %{nil}

%define name ultimarc
%define major 1
%define minor 1
%define release 14
%define version %{major}.%{minor}

Name: %{name}
Version: %{version}
Release: %{release}
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

./configure --prefix=/usr --libdir=%{_libdir}

%build
make clean
make

%install
rm -Rf %{buildroot}
%make_install

# udev rule - force /usr/lib/udev path because udev rules always lives there
install -d %{buildroot}/usr/lib/udev/rules.d
install -p 21-ultimarc.rules %{buildroot}/usr/lib/udev/rules.d/

#example json files
install -d %{buildroot}/%{_datarootdir}/ultimarc/examples
install -p -m 644 ./README.* %{buildroot}/%{_datarootdir}/ultimarc/
install -p -m 644 ./src/umtool/*.json %{buildroot}/%{_datarootdir}/ultimarc/examples/

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{_bindir}/umtool
%{_libdir}/libultimarc.*
/usr/lib/udev/rules.d/21-ultimarc.rules
%{_datarootdir}/ultimarc

%post
udevadm control --reload


%package devel
Requires: ultimarc
Summary: Ultimarc development headers and library files

%description devel
Ultimarc development header file and static library file

%files devel
%{_libdir}/pkgconfig/ultimarc-%{major}.%{minor}.pc
%{_includedir}/ultimarc
