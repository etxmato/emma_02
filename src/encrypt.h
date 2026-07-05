#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "wx/dir.h"
#include "wx/textfile.h"

typedef wxInt8   int8_t;
typedef wxUint8  uint8_t;
typedef wxInt16  int16_t;
typedef wxUint16 uint16_t;
typedef wxInt32  int32_t;
typedef wxUint32 uint32_t;

#if defined(__WXMAC__) || (__WXMSW__)
typedef wxInt64  int64_t;
typedef wxUint64  uint64_t;
#endif

class Encrypt
{
public:
    Encrypt ();
    ~Encrypt ();

    wxString encryptUrlData(wxString InputString);
    wxString encrypt(wxString InputString, wxString passwd);
    wxString md5(wxString inputString);

private:
    void to_bytes(uint32_t val, uint8_t *bytes);
    uint32_t to_int32(const uint8_t *bytes);

};

#endif  // ENCRYPT_H
