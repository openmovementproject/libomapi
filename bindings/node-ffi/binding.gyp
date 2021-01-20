{
	"targets": [
		{
			"target_name": "libomapi", 
			"sources": [ 
				"./src/omapi-node.cpp", 
				"./src/omapi-download.c", 
				"./src/omapi-internal.c", 
				"./src/omapi-main.c", 
				"./src/omapi-reader.c", 
				"./src/omapi-settings.c", 
				"./src/omapi-status.c", 
			],
			"include_dirs": [
				"./include", 
			],
			"conditions": [
				['OS=="win"', {
					'sources': [
						"./src/omapi-devicefinder-win.cpp", 
					],
					"product_dir": "Release", 
				},
				'OS=="linux"', {
					'sources': [
						"./src/omapi-devicefinder-linux.c", 
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
						"./src/omapi-devicefinder-mac.c", 
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
