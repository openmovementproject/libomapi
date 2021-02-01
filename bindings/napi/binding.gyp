{
	"targets": [
		{
			"target_name": "libomapi_napi", 
			"sources": [ 
				"src/interface.c",
				"src/nomapi.c",
				"src/nomapi-main.c",
				"src/nomapi-status.c",
				"src/nomapi-settings.c",
				"src/nomapi-download.c",
				"src/nomapi-reader.c",
				"../../src/omapi-main.c", 
				"../../src/omapi-status.c", 
				"../../src/omapi-settings.c", 
				"../../src/omapi-download.c", 
				"../../src/omapi-reader.c", 
				"../../src/omapi-internal.c", 
			],
			"include_dirs": [
				"../../include", 
			],
			"conditions": [
				['OS=="win"', {
					'sources': [
						"../../src/omapi-devicefinder-win.cpp", 
					],
					"product_dir": "Release", 
				},
				'OS=="linux"', {
					'sources': [
						"../../src/omapi-devicefinder-linux.c", 
					],
					'link_settings': {
						'libraries': [
							'-ludev',
							'-lpthread',
						],
					},
				},
				'OS=="mac"', {
					'sources': [
						"../../src/omapi-devicefinder-mac.c", 
					],
					'link_settings': {
						'libraries': [
							'-framework DiskArbitration',
						],
					},
				}],
			], 
			"product_dir": ""
		}
	],
}
