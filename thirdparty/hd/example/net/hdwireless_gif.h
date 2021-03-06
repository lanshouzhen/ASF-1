#ifndef HD_WIRELESS_GIF
#define HD_WIRELESS_GIF

static const char hdwireless_gif[] = {
        /*
         * "HTTP/1.0 200 OK\n"
         * "Server: lwIP\n"
         * "Content-type: image/gif\n\n",
         *
         */
        0x48, 0x54, 0x54, 0x50, 0x2f, 0x31, 0x2e, 0x30,
        0x20, 0x32, 0x30, 0x30, 0x20, 0x4f, 0x4b, 0x0d,
        0x0a, 0x53, 0x65, 0x72, 0x76, 0x65, 0x72, 0x3a,
        0x20, 0x6c, 0x77, 0x49, 0x50, 0x0d, 0x0a, 0x43,
        0x6f, 0x6e, 0x74, 0x65, 0x6e, 0x74, 0x2d, 0x74,
        0x79, 0x70, 0x65, 0x3a, 0x20, 0x69, 0x6d, 0x61,
        0x67, 0x65, 0x2f, 0x67, 0x69, 0x66, 0x0d, 0x0a,
        0x0d, 0x0a,

        /* gif data */
        0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x45, 0x01,
        0x7b, 0x00, 0xa1, 0x00, 0x00, 0xff, 0xff, 0xff,
        0x00, 0x66, 0x99, 0x00, 0x00, 0x00, 0xff, 0xff,
        0xff, 0x21, 0xfe, 0x11, 0x43, 0x72, 0x65, 0x61,
        0x74, 0x65, 0x64, 0x20, 0x77, 0x69, 0x74, 0x68,
        0x20, 0x47, 0x49, 0x4d, 0x50, 0x00, 0x2c, 0x00,
        0x00, 0x00, 0x00, 0x45, 0x01, 0x7b, 0x00, 0x00,
        0x02, 0xfe, 0x84, 0x8f, 0xa9, 0xcb, 0xed, 0x0f,
        0xa3, 0x9c, 0xb4, 0xda, 0x8b, 0xb3, 0xde, 0xbc,
        0xfb, 0x0f, 0x86, 0xe2, 0x48, 0x96, 0xe6, 0x89,
        0xa6, 0xea, 0xca, 0xb6, 0xee, 0x0b, 0xc7, 0xf2,
        0x4c, 0xd7, 0xf6, 0x8d, 0xe7, 0xfa, 0xce, 0xf7,
        0xfe, 0x0f, 0x0c, 0x0a, 0x87, 0xc4, 0xa2, 0xf1,
        0x88, 0x4c, 0x2a, 0x97, 0xcc, 0xa6, 0xf3, 0x09,
        0x8d, 0x4a, 0xa7, 0xd4, 0xaa, 0xf5, 0x8a, 0x65,
        0x05, 0xb6, 0xdc, 0xae, 0xf7, 0x0b, 0x0e, 0x8b,
        0xc7, 0x64, 0x71, 0xf6, 0x0c, 0x2d, 0xab, 0xd7,
        0xec, 0x36, 0x19, 0x0d, 0x3f, 0xba, 0xe7, 0xf4,
        0xfa, 0x3b, 0x8e, 0xff, 0xd9, 0xf7, 0x7c, 0x7b,
        0xfe, 0x9f, 0xd3, 0x27, 0x38, 0xc8, 0x06, 0x68,
        0x38, 0x43, 0x98, 0xa8, 0x18, 0x76, 0xd8, 0xd8,
        0xb2, 0x08, 0x19, 0x19, 0xe0, 0x48, 0x79, 0x22,
        0x79, 0xa9, 0x58, 0xa9, 0x29, 0x82, 0xd9, 0x29,
        0xb8, 0x09, 0xda, 0xe1, 0x39, 0xca, 0x17, 0x6a,
        0x8a, 0x41, 0x9a, 0x4a, 0x77, 0xca, 0x3a, 0xa1,
        0xfa, 0xea, 0xd6, 0x2a, 0xeb, 0x00, 0x5b, 0xab,
        0x36, 0x8b, 0xab, 0x60, 0xbb, 0x6b, 0x96, 0xeb,
        0x0b, 0xc0, 0x1b, 0xec, 0xf5, 0xeb, 0x2b, 0x6c,
        0xbc, 0x45, 0x8c, 0x7b, 0x6c, 0x9c, 0x3c, 0xbb,
        0x2c, 0xdc, 0x2c, 0xfb, 0x1c, 0x1c, 0xdd, 0x3a,
        0xcd, 0x5b, 0xcd, 0x7a, 0xbd, 0x9b, 0x7d, 0xba,
        0x6d, 0xdb, 0x6d, 0xfa, 0x5d, 0x1b, 0x1e, 0x3a,
        0x0e, 0x5b, 0x0e, 0x7a, 0xfe, 0x9a, 0xbe, 0xb9,
        0xae, 0xda, 0xae, 0xf9, 0x9e, 0x1a, 0x5f, 0x39,
        0x4f, 0x5a, 0x4f, 0x79, 0x3f, 0x9a, 0xef, 0xb8,
        0xef, 0xd9, 0xaf, 0xd1, 0xbf, 0x4e, 0x01, 0x0f,
        0x0d, 0xc4, 0x54, 0xd0, 0xd0, 0xc1, 0x4b, 0x09,
        0x01, 0x2d, 0x94, 0xd4, 0xf0, 0xcf, 0xc3, 0x48,
        0x11, 0xf3, 0x4c, 0x84, 0x54, 0x11, 0xcf, 0x1d,
        0xfe, 0x5d, 0x60, 0x1a, 0x8c, 0xf1, 0x58, 0xa6,
        0x42, 0x1b, 0x04, 0x02, 0x4a, 0x9a, 0x14, 0xb0,
        0xe0, 0xe4, 0xc9, 0x94, 0x2a, 0x51, 0x1e, 0x68,
        0x29, 0x03, 0x66, 0xb8, 0x8d, 0x09, 0x18, 0x31,
        0xb0, 0xf9, 0x80, 0xa6, 0xab, 0x5b, 0x24, 0x65,
        0xf6, 0xf4, 0xf9, 0x12, 0x28, 0x00, 0xa1, 0x2d,
        0x88, 0x36, 0xd3, 0x79, 0x00, 0x27, 0xc7, 0x61,
        0x12, 0x3e, 0x5e, 0xe0, 0x19, 0x54, 0xa5, 0x82,
        0x96, 0x25, 0xa7, 0x0a, 0x35, 0xba, 0x02, 0x2b,
        0x31, 0xa4, 0x06, 0x94, 0xd6, 0x64, 0xda, 0xb4,
        0xd7, 0x53, 0x9d, 0x54, 0x13, 0x50, 0x35, 0x69,
        0xf6, 0xaa, 0xd4, 0x18, 0x5a, 0x7f, 0x71, 0x05,
        0xd6, 0xf1, 0x26, 0xd8, 0xb0, 0x71, 0x51, 0x39,
        0x8d, 0xba, 0xf2, 0x67, 0x59, 0xbd, 0x68, 0xf9,
        0xba, 0x84, 0xd1, 0xb6, 0xd8, 0xdd, 0xaf, 0x5f,
        0x40, 0x72, 0x91, 0x3b, 0x69, 0x69, 0xe1, 0x0c,
        0x64, 0xd5, 0x9e, 0x4d, 0xbb, 0x76, 0x47, 0xe0,
        0x5c, 0x6f, 0xbd, 0x22, 0x98, 0x7b, 0x19, 0x99,
        0x62, 0xcc, 0x49, 0x0f, 0xd3, 0x12, 0x8b, 0xb7,
        0xaf, 0x81, 0xb3, 0x55, 0xfd, 0xfa, 0x98, 0xac,
        0x6c, 0x70, 0xe6, 0xc5, 0x0b, 0x58, 0xaf, 0xde,
        0xcc, 0xb9, 0x6b, 0x6c, 0xd9, 0x96, 0xf7, 0x0e,
        0x25, 0x5d, 0xfa, 0x76, 0xe4, 0xd0, 0x7f, 0x47,
        0xfb, 0xcc, 0xcb, 0x12, 0x78, 0x83, 0xc9, 0xa8,
        0x79, 0xe7, 0x7e, 0xf0, 0xb8, 0x47, 0xe5, 0xba,
        0xb0, 0x5b, 0x27, 0x26, 0xcc, 0x7c, 0x75, 0x17,
        0xc4, 0xd1, 0x75, 0x03, 0xc7, 0x9d, 0xdb, 0x36,
        0x6f, 0xc8, 0x2b, 0x89, 0x62, 0xef, 0xcd, 0xfd,
        0x78, 0x78, 0xed, 0x56, 0xb1, 0x0f, 0xff, 0xce,
        0xe3, 0xad, 0xc8, 0xd9, 0xce, 0x41, 0x4b, 0xf7,
        0xdc, 0xde, 0xb5, 0xef, 0xc8, 0xe8, 0xe7, 0x0b,
        0xb7, 0x2f, 0xbe, 0x2c, 0xd0, 0xef, 0xf7, 0xfe,
        0xf1, 0x83, 0xf7, 0x8f, 0x5b, 0x70, 0xfc, 0x95,
        0x37, 0x60, 0x20, 0xaa, 0xd9, 0xc5, 0xde, 0x7b,
        0xd5, 0xd1, 0x36, 0x1d, 0x75, 0xb1, 0xfd, 0x66,
        0x1e, 0x80, 0x04, 0xe6, 0x57, 0x20, 0x80, 0x01,
        0x8e, 0x27, 0x60, 0x85, 0x16, 0x92, 0x36, 0x5e,
        0x84, 0x38, 0xa8, 0x47, 0xc1, 0x82, 0xd0, 0xb9,
        0xd7, 0x19, 0x7b, 0x24, 0x5a, 0x87, 0x96, 0x4c,
        0x2a, 0xee, 0xb6, 0x9d, 0x5f, 0xda, 0xf1, 0xc7,
        0x22, 0x8a, 0xff, 0xc1, 0x78, 0x9f, 0x63, 0xc2,
        0xd9, 0x28, 0x9a, 0x0d, 0x20, 0xee, 0x04, 0x5f,
        0x4e, 0x21, 0xc5, 0xd7, 0xe3, 0x88, 0x0d, 0x76,
        0x28, 0xa3, 0x7f, 0x32, 0xfe, 0x57, 0xa4, 0x8b,
        0x10, 0xc6, 0x48, 0x1e, 0x8e, 0xe2, 0x1d, 0x69,
        0xda, 0x04, 0xc5, 0xdd, 0xb0, 0x23, 0x5d, 0xcf,
        0x41, 0x00, 0x9f, 0x88, 0x22, 0xc2, 0xb5, 0xa0,
        0x7e, 0x6b, 0xc1, 0x44, 0x9e, 0x91, 0x4a, 0x22,
        0x89, 0x95, 0x93, 0x18, 0x66, 0xd8, 0x5b, 0x98,
        0xe7, 0xc5, 0xe8, 0x43, 0x95, 0x11, 0x04, 0x89,
        0xe0, 0x95, 0x0a, 0xca, 0x39, 0xa7, 0x66, 0x1d,
        0xda, 0xa6, 0x61, 0x8b, 0xc6, 0x21, 0x79, 0x24,
        0x8c, 0x51, 0xee, 0x59, 0x61, 0x9e, 0x63, 0xf6,
        0xf7, 0xe1, 0x81, 0x0c, 0x0e, 0xb9, 0x14, 0x07,
        0xb3, 0x6d, 0x79, 0x62, 0x9f, 0x37, 0x7a, 0x78,
        0xe6, 0x9e, 0x0c, 0xd0, 0xb8, 0x1f, 0x8b, 0x94,
        0xee, 0x46, 0x23, 0x9a, 0x1c, 0xea, 0xb0, 0x9c,
        0x7c, 0x99, 0x21, 0xc6, 0x23, 0x9d, 0x87, 0xda,
        0x09, 0x1b, 0x9c, 0x1b, 0xe6, 0x58, 0xdf, 0x84,
        0xe0, 0xa9, 0x79, 0x2a, 0xa4, 0x4e, 0x66, 0x3a,
        0x28, 0x9b, 0xb0, 0x16, 0xda, 0xa8, 0x65, 0x9d,
        0x01, 0xf9, 0x26, 0xa2, 0x73, 0x3a, 0x38, 0x5b,
        0xaa, 0x8e, 0xe6, 0xf8, 0xa7, 0x84, 0x93, 0x5e,
        0x8a, 0xaa, 0xa5, 0xc4, 0x3e, 0xf9, 0xfe, 0x2b,
        0xa1, 0xc9, 0x02, 0x4b, 0x43, 0xa7, 0x26, 0x8a,
        0xea, 0xa9, 0x90, 0xbc, 0x1a, 0x26, 0x22, 0xa4,
        0xad, 0x0e, 0x5b, 0xa3, 0x56, 0xac, 0x3a, 0x60,
        0xa6, 0x07, 0x9b, 0x22, 0xf7, 0x6d, 0x0d, 0xce,
        0xea, 0xfa, 0x29, 0x90, 0xa4, 0x6e, 0x46, 0x9d,
        0x8f, 0xb6, 0x9e, 0xaa, 0x2a, 0x9b, 0x49, 0x1a,
        0xb7, 0xa6, 0xb2, 0xdc, 0x1a, 0xeb, 0x6e, 0x06,
        0x53, 0xbe, 0xab, 0xa3, 0xa1, 0x5c, 0x3e, 0x6b,
        0x18, 0x8f, 0x1b, 0xe8, 0xfb, 0x6b, 0xbb, 0xcc,
        0xea, 0x29, 0xac, 0xa6, 0xab, 0x76, 0x2b, 0xa9,
        0x8b, 0x14, 0x6c, 0xab, 0xaa, 0x81, 0xb5, 0x32,
        0x2a, 0x2a, 0x63, 0x11, 0x5b, 0xb9, 0xe5, 0x85,
        0x83, 0xc6, 0x4b, 0x61, 0xbd, 0xcb, 0x7a, 0xc7,
        0x24, 0xb1, 0x09, 0xeb, 0x99, 0x1c, 0xc1, 0xe2,
        0xea, 0xbb, 0x2e, 0x97, 0x1e, 0x98, 0x4a, 0x71,
        0xc5, 0x21, 0xdf, 0xc9, 0x27, 0xbe, 0x50, 0x62,
        0x2c, 0x28, 0xc7, 0x94, 0xb2, 0x1c, 0xae, 0xc7,
        0xb4, 0xae, 0x5b, 0x32, 0xb9, 0x16, 0xe8, 0x9c,
        0xab, 0x7a, 0xe1, 0x8e, 0x09, 0x33, 0x91, 0xf3,
        0xfa, 0xd9, 0xb0, 0xc0, 0x2b, 0xb7, 0xfa, 0xaa,
        0xc5, 0x54, 0x92, 0x0c, 0x31, 0xca, 0xed, 0xbd,
        0x37, 0xd6, 0x8f, 0xd8, 0xba, 0xfb, 0x73, 0xb0,
        0x2f, 0x83, 0xab, 0x74, 0xa4, 0x46, 0x0f, 0x0c,
        0x28, 0xd5, 0x82, 0x8e, 0xfc, 0x70, 0xb4, 0xa3,
        0x4e, 0xbc, 0x6f, 0x83, 0x09, 0x96, 0x0a, 0xf0,
        0xd5, 0x9a, 0x0e, 0x0d, 0x2c, 0xc3, 0x45, 0xb3,
        0x8d, 0xac, 0xd0, 0x11, 0x1c, 0x0d, 0x73, 0xcb,
        0xf9, 0x5e, 0x94, 0x48, 0x46, 0x71, 0xe0, 0x9d,
        0x89, 0xde, 0x68, 0xf0, 0x9d, 0xb7, 0xdf, 0x67,
        0x00, 0x4e, 0x88, 0xe0, 0x83, 0x13, 0xfe, 0x89,
        0xe1, 0x58, 0x20, 0x9e, 0xb8, 0xe2, 0x56, 0x30,
        0xde, 0x87, 0xe3, 0x57, 0x40, 0xad, 0x5e, 0x8a,
        0xe4, 0x55, 0x50, 0xbe, 0x87, 0xe5, 0x97, 0x63,
        0x5e, 0x87, 0xe6, 0x54, 0x70, 0xde, 0xb9, 0xe7,
        0x52, 0x80, 0xbe, 0x8a, 0xe8, 0x51, 0x90, 0x3e,
        0x87, 0xe9, 0xa7, 0xa3, 0x3e, 0x92, 0xea, 0x4f,
        0xb0, 0xde, 0xba, 0xeb, 0x4d, 0xc0, 0x5e, 0x88,
        0xec, 0xb3, 0xd3, 0x0e, 0x95, 0xed, 0x4a, 0xe0,
        0x9e, 0xbb, 0xee, 0x48, 0xf0, 0x2e, 0xb5, 0xef,
        0xbf, 0x03, 0x9f, 0xb6, 0xf0, 0x40, 0x10, 0x5f,
        0xbc, 0xf1, 0x6d, 0x22, 0x5f, 0xb2, 0xf2, 0x7a,
        0x30, 0xbf, 0xa5, 0xf3, 0xcb, 0x43, 0x7f, 0xb6,
        0xf4, 0x3b, 0x50, 0x5f, 0xbd, 0xf5, 0x9c, 0x62,
        0xef, 0xb4, 0xf6, 0xca, 0x71, 0x4f, 0xb6, 0xf7,
        0xd7, 0x83, 0x2f, 0x3e, 0x11, 0xe4, 0x97, 0x6f,
        0x3e, 0xf6, 0xe8, 0xa7, 0x4f, 0xfd, 0xfa, 0xec,
        0x23, 0xef, 0x7e, 0x11, 0xcc, 0xc7, 0x2f, 0x3f,
        0xf1, 0xf4, 0x1b, 0x01, 0xfc, 0xfd, 0xc3, 0xb3,
        0xae, 0x7f, 0x12, 0xa4, 0xf7, 0x7f, 0x3b, 0xc6,
        0x01, 0xf0, 0x73, 0x07, 0x19, 0xa0, 0x01, 0x0f,
        0x88, 0xc0, 0x04, 0x2a, 0x70, 0x81, 0x0c, 0x6c,
        0xa0, 0x03, 0x1f, 0x08, 0xc1, 0x08, 0x4a, 0x70,
        0x82, 0x14, 0xac, 0xa0, 0x05, 0x2f, 0x88, 0x41,
        0xcd, 0x15, 0x00, 0x00, 0x3b
};

#endif
