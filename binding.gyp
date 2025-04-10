{
    "targets": [
        {
            "target_name": "speechrecorder",
            "sources": ["src/speech_recorder.cpp"],
            "cflags!": [
                "-fno-exceptions",
                "-fno-rtti"
            ],
            "cflags_cc!": [
                "-fno-exceptions",
                "-fno-rtti"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<(module_root_dir)/include",
                "<(module_root_dir)/lib/include",
                "<(module_root_dir)/lib/build/_deps/drwav-src",
                "<(module_root_dir)/lib/build/_deps/readerwriterqueue-src",
                "<(module_root_dir)/lib/3rd_party/portaudio/include"
            ],
            "defines": [
                "NAPI_VERSION=<(napi_build_version)",
                "NAPI_CPP_EXCEPTIONS"
            ],
            "conditions": [
                [
                    'OS=="mac"',
                    {
                        "xcode_settings": {
                            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                            "GCC_ENABLE_CPP_RTTI": "YES",
                            "MACOSX_DEPLOYMENT_TARGET": "10.14",
                            "OTHER_LDFLAGS": ["-Wl,-rpath,@loader_path/"]
                        },
                        "copies": [
                            {
                                "destination": "<(module_root_dir)/build/Release",
                                "files": [
                                    "<(module_root_dir)/lib/install/lib/libspeechrecorder.dylib",
                                    "<(module_root_dir)/lib/install/lib/libportaudio.dylib"
                                ]
                            }
                        ],
                        "libraries": [
                            "<(module_root_dir)/build/Release/libspeechrecorder.dylib",
                            "<(module_root_dir)/build/Release/libportaudio.dylib"
                        ]
                    }
                ],
                [
                    'OS=="win"',
                    {
                        "msvs_settings": {
                            "VCCLCompilerTool": {
                                "ExceptionHandling": 1
                            }
                        },
                        "copies": [
                            {
                                "destination": "<(module_root_dir)/build/Release",
                                "files": [
                                    "<(module_root_dir)/lib/install/lib/speechrecorder.dll"
                                ]
                            }
                        ],
                        "libraries": [
                            "<(module_root_dir)/lib/install/lib/speechrecorder.lib"
                        ]
                    }
                ],
                [
                    'OS=="win" and target_arch=="ia32"',
                    {
                        "copies": [
                            {
                                "destination": "<(module_root_dir)/build/Release",
                                "files": [
                                    "<(module_root_dir)/lib/install/lib/portaudio_x86.dll",
                                    "<(module_root_dir)/lib/3rd_party/vcruntime/x86/vcruntime140.dll"
                                ]
                            }
                        ],
                        "libraries": [
                            "<(module_root_dir)/lib/install/lib/portaudio_x86.lib"
                        ]
                    }
                ],
                [
                    'OS=="win" and target_arch=="x64"',
                    {
                        "copies": [
                            {
                                "destination": "<(module_root_dir)/build/Release",
                                "files": [
                                    "<(module_root_dir)/lib/install/lib/portaudio_x64.dll",
                                    "<(module_root_dir)/lib/3rd_party/vcruntime/x64/vcruntime140.dll",
                                    "<(module_root_dir)/lib/3rd_party/vcruntime/x64/vcruntime140_1.dll"
                                ]
                            }
                        ],
                        "libraries": [
                            "<(module_root_dir)/lib/install/lib/portaudio_x64.lib"
                        ]
                    }
                ],
                [
                    'OS=="linux"',
                    {
                        "link_settings": {
                            "libraries": [
                                "-Wl,-rpath,'$$ORIGIN'"
                            ]
                        },
                        "copies": [
                            {
                                "destination": "<(module_root_dir)/build/Release",
                                "files": [
                                    "<(module_root_dir)/lib/install/lib/libspeechrecorder.so",
                                    "<(module_root_dir)/lib/install/lib/libportaudio.so"
                                ]
                            }
                        ],
                        "libraries": [
                            "<(module_root_dir)/build/Release/libspeechrecorder.so",
                            "<(module_root_dir)/build/Release/libportaudio.so"
                        ]
                    }
                ]
            ]
        }
    ]
}