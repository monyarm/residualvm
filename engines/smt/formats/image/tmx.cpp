#include "smt/formats/image/tmx.h"

void TMXFile::ReadFile(char *path)
{
    Common::File f;
    if (f.open(path))
    {

        dat.header.tmxID = f.readUint16BE();
        dat.header.userID = f.readUint16BE();
        dat.header.length = f.readUint32BE();

        f.read(&dat.header.tmxMagic, 4);
        dat.header.pad1 = f.readUint32BE();

        dat.formatsettings.numPalettes = f.readByte();
        dat.formatsettings.paletteFmt = (PS2PixelFormat)f.readByte();
        dat.formatsettings.width = f.readUint16LE();
        dat.formatsettings.height = f.readUint16LE();
        dat.formatsettings.pixelFmt = (PS2PixelFormat)f.readByte();
        dat.formatsettings.numMipMaps = f.readByte();
        dat.formatsettings.mipKL = f.readUint16BE();
        dat.formatsettings.reserved = f.readByte();
        dat.formatsettings.wrapModes = f.readByte();
        dat.formatsettings.userTextureID = f.readUint32BE();
        dat.formatsettings.userClutID = f.readUint32LE();

        f.read(&dat.formatsettings.userComment, 28);

        Common::DumpFile pal;
        Common::DumpFile img;


        switch (dat.formatsettings.pixelFmt)
        {
        case PSMT4:
        {
            /* pal.open("dumps/pal.data", true);
            img.open("dumps/image.data", true);
 */
            byte *_pixels = new byte[(sizeof(byte) * dat.formatsettings.width * dat.formatsettings.height) / 2];

            f.read(_pixels, (sizeof(byte) * dat.formatsettings.width * dat.formatsettings.height) / 2);

            Common::Array<byte> pixels;
            for (size_t i = 0; i < (sizeof(byte) * dat.formatsettings.width * dat.formatsettings.height); i++)
            {

                pixels.push_back(_pixels[i] & 0x0f);
                pixels.push_back(_pixels[i] >> 4);
            }
/* 
            img.write(pixels.data(), (sizeof(byte) * dat.formatsettings.width * dat.formatsettings.height));

            img.flush();
            img.close(); */
            delete _pixels;
        }
        break;
        case PSMT8:
        {
            pal.open("dumps/pal.data", true);
            img.open("dumps/image.data", true); 

            byte *_palette = new byte[256 * 4];
            f.read(_palette, 256 * 4);

            Common::Array<byte> palette;

            int index = 0;
            for (int i = 0; i < 8; i++)
            {
                for (int x = 0; x < 8; x++)
                {
                    palette.push_back(_palette[(index + x) * 4 + 0]);
                    palette.push_back(_palette[(index + x) * 4 + 1]);
                    palette.push_back(_palette[(index + x) * 4 + 2]);
                    palette.push_back(_palette[(index + x) * 4 + 3]);
                }
                index += 16;

                for (int x = 0; x < 8; x++)
                {

                    palette.push_back(_palette[(index + x) * 4 + 0]);
                    palette.push_back(_palette[(index + x) * 4 + 1]);
                    palette.push_back(_palette[(index + x) * 4 + 2]);
                    palette.push_back(_palette[(index + x) * 4 + 3]);
                }
                index -= 8;

                for (int x = 0; x < 8; x++)
                {

                    palette.push_back(_palette[(index + x) * 4 + 0]);
                    palette.push_back(_palette[(index + x) * 4 + 1]);
                    palette.push_back(_palette[(index + x) * 4 + 2]);
                    palette.push_back(_palette[(index + x) * 4 + 3]);
                }
                index += 16;

                for (int x = 0; x < 8; x++)
                {

                    palette.push_back(_palette[(index + x) * 4 + 0]);
                    palette.push_back(_palette[(index + x) * 4 + 1]);
                    palette.push_back(_palette[(index + x) * 4 + 2]);
                    palette.push_back(_palette[(index + x) * 4 + 3]);
                }
                index += 8;
            }/* 
            pal.write(palette.data(), 256 * 4);
            pal.flush();
            pal.close(); */

            byte *pixels = new byte[(sizeof(byte) * dat.formatsettings.width * dat.formatsettings.height)];

            f.read(pixels, sizeof(PS2PixelFormat) * dat.formatsettings.width * dat.formatsettings.height);

            /* img.write(pixels, sizeof(PS2PixelFormat) * dat.formatsettings.width * dat.formatsettings.height);

            img.flush();
            img.close(); */
            //dat.palette = palette;
            //dat.image.assign(pixels, pixels + dat.formatsettings.width * dat.formatsettings.height);

            Common::Array<uint32> image;
            uint32 *truepalette = (uint32 *)palette.data();
            for (size_t i = 0; i < dat.formatsettings.width * dat.formatsettings.height ; i++)
            {
                image.push_back(truepalette[pixels[i]]);
            }
            img.write(image.data(), dat.formatsettings.width * dat.formatsettings.height * sizeof(uint32));
            img.flush();
            img.close();

            surface = new Graphics::Surface();
            surface->w = dat.formatsettings.width;
            surface->h = dat.formatsettings.height;
            debug(Graphics::createPixelFormat<8888>().toString().c_str());
            surface->format = Graphics::createPixelFormat<8888>();
            surface->setPixels(image.data());
        }
        break;
        case PSMTC32:
            break;
        case PSMTC24:
            break;
        case PSMTC16:
            break;
        case PSMTC16S:
            break;
        case PSMT8H:
            break;
        case PSMT4HL:
            break;
        case PSMT4HH:
            break;
        case PSMZ32:
            break;
        case PSMZ24:
            break;
        case PSMZ16:
            break;
        case PSMZ16S:
            break;

        default:
            break;
        }
    }
}

Graphics::Surface *TMXFile::getSurface() const
{
    return surface;
}
TMXFile::TMXFile()
{
}
