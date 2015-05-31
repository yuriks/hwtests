rem Input formats
set SCALE=-vf scale=out_color_matrix=bt709:out_range=tv
ffmpeg -i mandrill.png -pix_fmt yuv422p %SCALE% mandrill_422p.yuv
ffmpeg -i mandrill.png -pix_fmt yuv420p %SCALE% mandrill_420p.yuv
ffmpeg -i mandrill.png -pix_fmt yuv422p16be %SCALE% mandrill_422p16.yuv
ffmpeg -i mandrill.png -pix_fmt yuv420p16be %SCALE% mandrill_420p16.yuv
ffmpeg -i mandrill.png -pix_fmt yuyv422 %SCALE% mandrill_yuyv422.yuv

rem Chroma upscaling test
ffmpeg -i vivio.png -pix_fmt yuv420p %SCALE% vivio_420p.yuv

rem Matrix tests
ffmpeg -i mandrill.png -pix_fmt yuv420p -vf scale=out_color_matrix=bt709:out_range=pc mandrill_420p_bt709f.yuv
ffmpeg -i mandrill.png -pix_fmt yuv420p -vf scale=out_color_matrix=bt601:out_range=tv mandrill_420p_bt601.yuv
ffmpeg -i mandrill.png -pix_fmt yuv420p -vf scale=out_color_matrix=bt601:out_range=pc mandrill_420p_bt601f.yuv
