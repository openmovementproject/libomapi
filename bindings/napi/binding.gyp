{
	"targets": [
		{
			"target_name": "libomapi_napi", 
			"sources": [ 
				"./src/interface.c",
				"./src/nomapi.c",
				"./src/nomapi-main.c",
				"./src/nomapi-status.c",
				"./src/nomapi-settings.c",
				"./src/nomapi-download.c",
				"./src/nomapi-reader.c",
				"./libomapi/src/omapi-main.c", 
				"./libomapi/src/omapi-status.c", 
				"./libomapi/src/omapi-settings.c", 
				"./libomapi/src/omapi-download.c", 
				"./libomapi/src/omapi-reader.c", 
				"./libomapi/src/omapi-internal.c", 
			],
			"include_dirs": [
				"./libomapi/include", 
			],
			"conditions": [
				['OS=="win"', {
					'sources': [
						"./libomapi/src/omapi-devicefinder-win.cpp", 
					],
					"product_dir": "Release", 
				},
				'OS=="linux"', {
					'sources': [
						"./libomapi/src/omapi-devicefinder-linux.c", 
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
						"./libomapi/src/omapi-devicefinder-mac.c", 
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
