#Copyright (c) 2019-2020 <>< Charles Lohr - Under the MIT/x11 or NewBSD License you choose.
# NO WARRANTY! NO GUARANTEE OF SUPPORT! USE AT YOUR OWN RISK

all : makecapk.apk

.PHONY : push run

COLOUR_GREEN=\033[0;32m
COLOUR_RED=\033[0;31m
COLOUR_PURPLE=\033[0;35m
COLOUR_END=\033[0m

UPLOAD_COUNT=$(file < upload_count.txt)

increment_upload_count:
	$(eval UPLOAD_COUNT=$(shell echo $$(($(UPLOAD_COUNT)+1))))
	@echo -e "$(COLOUR_PURPLE)Count of all the uploads to my quest 3 $(UPLOAD_COUNT) !!!!!$(COLOUR_END)"
	@echo $(UPLOAD_COUNT) > upload_count.txt

COMPILE_COUNT=$(file < compile_count.txt)

# WARNING WARNING WARNING!  YOU ABSOLUTELY MUST OVERRIDE THE PROJECT NAME
# you should also override these parameters, get your own signatre file and make your own manifest.
APPNAME?=DuniaDemo
LABEL?=$(APPNAME)
APKFILE ?= $(APPNAME).apk
PACKAGENAME?=org.LepidopteraStudio.$(APPNAME)
RAWDRAWANDROID?=lib/
RAWDRAWANDROIDSRCS=$(RAWDRAWANDROID)/android_native_app_glue.c
SRC?=main.c
OXR_HEADERS?=lib/openxr/include
TSOPENXR_HEADERS?=tsopenxr/
ENGINE_SOURCES?= engine/*.cpp lib/android_native_app_glue.c lib/volk.c

#We've tested it with android version 22, 24, 28, 29 and 30.
#You can target something like Android 28, but if you set ANDROIDVERSION to say 22, then
#Your app should (though not necessarily) support all the way back to Android 22. 
ANDROIDVERSION=30
ANDROIDTARGET=30
#$(ANDROIDVERSION)
#Default is to be strip down, but your app can override it.
CFLAGS?=-ffunction-sections -O2 -fdata-sections -Wall -fvisibility=hidden -I..
LDFLAGS?=-Wl,--gc-sections -s
ANDROID_FULLSCREEN?=y
ADB?=/home/clara/Android/Sdk/platform-tools/adb
UNAME := $(shell uname)

ANDROIDSRCS:= $(SRC) $(RAWDRAWANDROIDSRCS) lib/openxr_fb/libopenxr_loader.so

#if you have a custom Android Home location you can add it to this list.  
#This makefile will select the first present folder.

ifeq ($(UNAME), Linux)
OS_NAME = linux-x86_64
endif
ifeq ($(UNAME), Darwin)
OS_NAME = darwin-x86_64
endif
ifeq ($(OS), Windows_NT)
OS_NAME = windows-x86_64
endif

# Search list for where to try to find the SDK
SDK_LOCATIONS += $(ANDROID_HOME) $(ANDROID_SDK_ROOT) ~/Android/Sdk $(HOME)/Library/Android/sdk

#Just a little Makefile witchcraft to find the first SDK_LOCATION that exists
#Then find an ndk folder and build tools folder in there.
ANDROIDSDK?=$(firstword $(foreach dir, $(SDK_LOCATIONS), $(basename $(dir) ) ) )
NDK?=$(firstword $(ANDROID_NDK) $(ANDROID_NDK_HOME) $(wildcard $(ANDROIDSDK)/ndk/*) $(wildcard $(ANDROIDSDK)/ndk-bundle/*) )
BUILD_TOOLS?=$(lastword $(wildcard $(ANDROIDSDK)/build-tools/*) )

# fall back to default Android SDL installation location if valid NDK was not found
ifeq ($(NDK),)
ANDROIDSDK := ~/Android/Sdk
endif

# Verify if directories are detected
ifeq ($(ANDROIDSDK),)
$(error ANDROIDSDK directory not found)
endif
ifeq ($(NDK),)
$(error NDK directory not found)
endif
ifeq ($(BUILD_TOOLS),)
$(error BUILD_TOOLS directory not found)
endif

testsdk :
	@echo "SDK:\t\t" $(ANDROIDSDK)
	@echo "NDK:\t\t" $(NDK)
	@echo "Build Tools:\t" $(BUILD_TOOLS)

CFLAGS+=-O2 -DANDROID -DAPPNAME=\"$(APPNAME)\"
ifeq (ANDROID_FULLSCREEN,y)
CFLAGS +=-DANDROID_FULLSCREEN
endif
CFLAGS+= -I$(RAWDRAWANDROID)/rawdraw -I$(OXR_HEADERS) -I$(TSOPENXR_HEADERS) -I$(NDK)/sysroot/usr/include -I$(NDK)/sysroot/usr/include/android -I$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/include/android -fPIC -I$(RAWDRAWANDROID) -DANDROIDVERSION=$(ANDROIDVERSION)
LDFLAGS += -lm -lGLESv3 -lEGL -landroid -llog
LDFLAGS += -shared -uANativeActivity_onCreate

CC_ARM64:=$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/bin/aarch64-linux-android$(ANDROIDVERSION)-clang
CC_ARM32:=$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/bin/armv7a-linux-androideabi$(ANDROIDVERSION)-clang
CC_x86:=$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/bin/i686-linux-android$(ANDROIDVERSION)-clang
CC_x86_64=$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/bin/x86_64-linux-android$(ANDROIDVERSION)-clang
AAPT:=$(BUILD_TOOLS)/aapt

# Which binaries to build? Just comment/uncomment these lines:
TARGETS += makecapk/lib/arm64-v8a/libDisrupt_b64.so
#TARGETS += makecapk/lib/arm64-v8a/lib$(APPNAME).so
#TARGETS += makecapk/lib/armeabi-v7a/lib$(APPNAME).so
#TARGETS += makecapk/lib/x86/lib$(APPNAME).so
#TARGETS += makecapk/lib/x86_64/lib$(APPNAME).so

CFLAGS_ARM64:=-m64
CFLAGS_ARM32:=-mfloat-abi=softfp -m32
CFLAGS_x86:=-march=i686 -mtune=intel -mssse3 -mfpmath=sse -m32
CFLAGS_x86_64:=-march=x86-64 -msse4.2 -mpopcnt -m64 -mtune=intel
STOREPASS?=password
DNAME:="CN=example.com, OU=ID, O=Example, L=Doe, S=John, C=GB"
KEYSTOREFILE:=~/jarsigner/keystore_oxr_devel.jks
ALIASNAME?=LS_devel

keystore : $(KEYSTOREFILE)

$(KEYSTOREFILE) :
	keytool -genkey -v -keystore $(KEYSTOREFILE) -alias $(ALIASNAME) -keyalg RSA -keysize 2048 -validity 10000 -storepass $(STOREPASS) -keypass $(STOREPASS) -dname $(DNAME)

folders:
	mkdir -p makecapk/lib/arm64-v8a
	mkdir -p makecapk/lib/armeabi-v7a
	mkdir -p makecapk/lib/x86
	mkdir -p makecapk/lib/x86_64

CPP_ARM64 := $(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/bin/aarch64-linux-android$(ANDROIDVERSION)-clang++
CPPFLAGS_ARM64 := -m64
CPP_LIB_SO := $(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/aarch64-linux-android/libc++_shared.so
CPPFLAGS := -I./ -Ilib/openxr/include -std=c++17 -nostdinc++ -fvisibility=hidden -O2
CPPLINK :=-Wl,-landroid -llog -shared -lopenxr_loader

#TODO(clara): Make this not happen in the release build

makecapk/lib/arm64-v8a/libDisrupt_b64.so : $(ENGINE_SOURCES)
	$(eval COMPILE_COUNT=$(shell echo $$(($(COMPILE_COUNT)+1))))
	@echo -e "$(COLOUR_PURPLE)Count of all engine compilation $(COMPILE_COUNT) !!!!!$(COLOUR_END)"
	@echo $(COMPILE_COUNT) > compile_count.txt
	mkdir -p makecapk/lib/arm64-v8a
	$(CPP_ARM64) $(CPPFLAGS_ARM64) $(CPPFLAGS) -o $@ $^ \
	-Llib/openxr_fb -L$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/aarch64-linux-android/$(ANDROIDVERSION) $(CPPLINK)

makecapk/lib/arm64-v8a/lib$(APPNAME).so : $(ANDROIDSRCS)
	mkdir -p makecapk/lib/arm64-v8a
	$(CC_ARM64) $(CFLAGS) $(CFLAGS_ARM64) -o $@ $^ -L$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/aarch64-linux-android/$(ANDROIDVERSION) $(LDFLAGS)

#makecapk/lib/armeabi-v7a/lib$(APPNAME).so : $(ANDROIDSRCS)
#	mkdir -p makecapk/lib/armeabi-v7a
#	$(CC_ARM32) $(CFLAGS) $(CFLAGS_ARM32) -o $@ $^ -L$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/arm-linux-androideabi/$(ANDROIDVERSION) $(LDFLAGS)

#makecapk/lib/x86/lib$(APPNAME).so : $(ANDROIDSRCS)
#	mkdir -p makecapk/lib/x86
#	$(CC_x86) $(CFLAGS) $(CFLAGS_x86) -o $@ $^ -L$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/i686-linux-android/$(ANDROIDVERSION) $(LDFLAGS)

#makecapk/lib/x86_64/lib$(APPNAME).so : $(ANDROIDSRCS)
#	mkdir -p makecapk/lib/x86_64
#	$(CC_x86) $(CFLAGS) $(CFLAGS_x86_64) -o $@ $^ -L$(NDK)/toolchains/llvm/prebuilt/$(OS_NAME)/sysroot/usr/lib/x86_64-linux-android/$(ANDROIDVERSION) $(LDFLAGS)

#We're really cutting corners.  You should probably use resource files.. Replace android:label="@string/app_name" and add a resource file.
#Then do this -S Sources/res on the aapt line.
#For icon support, add -S makecapk/res to the aapt line.  also,  android:icon="@mipmap/icon" to your application line in the manifest.
#If you want to strip out about 800 bytes of data you can remove the icon and strings.

#Notes for the past:  These lines used to work, but don't seem to anymore.  Switched to newer jarsigner.
#(zipalign -c -v 8 makecapk.apk)||true #This seems to not work well.
#jarsigner -verify -verbose -certs makecapk.apk



makecapk.apk : $(TARGETS) $(EXTRA_ASSETS_TRIGGER) AndroidManifest.xml
	mkdir -p makecapk/assets
	cp -r apksrc/assets/* makecapk/assets
	cp lib/openxr_fb/libopenxr_loader.so makecapk/lib/arm64-v8a/
	cp $(CPP_LIB_SO) makecapk/lib/arm64-v8a/
	rm -rf temp.apk
	$(AAPT) package -f -F temp.apk -I $(ANDROIDSDK)/platforms/android-$(ANDROIDVERSION)/android.jar -M AndroidManifest.xml -S apksrc/res -A makecapk/assets -v --target-sdk-version $(ANDROIDTARGET)
	unzip -o temp.apk -d makecapk
	rm -rf makecapk.apk
	cd makecapk && zip -D9r ../makecapk.apk . && zip -D0r ../makecapk.apk ./resources.arsc ./AndroidManifest.xml
	jarsigner -sigalg SHA1withRSA -digestalg SHA1 -verbose -keystore $(KEYSTOREFILE) -storepass $(STOREPASS) makecapk.apk $(ALIASNAME)
	rm -rf $(APKFILE)
	$(BUILD_TOOLS)/zipalign -v 4 makecapk.apk $(APKFILE)
	#Using the apksigner in this way is only required on Android 30+
	$(BUILD_TOOLS)/apksigner sign --key-pass pass:$(STOREPASS) --ks-pass pass:$(STOREPASS) --ks $(KEYSTOREFILE) $(APKFILE)
	rm -rf temp.apk
	rm -rf makecapk.apk
	@ls -l $(APKFILE)

manifest: AndroidManifest.xml

AndroidManifest.xml :
	rm -rf AndroidManifest.xml
	PACKAGENAME=$(PACKAGENAME) \
		ANDROIDVERSION=$(ANDROIDVERSION) \
		ANDROIDTARGET=$(ANDROIDTARGET) \
		APPNAME=$(APPNAME) \
		LABEL=$(LABEL) envsubst '$$ANDROIDTARGET $$ANDROIDVERSION $$APPNAME $$PACKAGENAME $$LABEL' \
		< AndroidManifest.xml.template > AndroidManifest.xml


uninstall : 
	($(ADB) uninstall $(PACKAGENAME))||true

push : makecapk.apk increment_upload_count
	@echo "Installing" $(PACKAGENAME)
	$(ADB) install -r $(APKFILE)

run : push
	$(eval ACTIVITYNAME:=$(shell $(AAPT) dump badging $(APKFILE) | grep "launchable-activity" | cut -f 2 -d"'"))
	$(ADB) shell am start -n $(PACKAGENAME)/$(ACTIVITYNAME)

stop :
	$(ADB) shell am force-stop $(PACKAGENAME)

clean :
	rm -rf temp.apk makecapk.apk makecapk $(APKFILE) $(APKFILE).idsig libengine.a
