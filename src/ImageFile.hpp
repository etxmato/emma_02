//++
// ImageFile.hpp -> CImageFile (image file I/O for the emulator library) class
//                  CDiskImageFile (disk image file) subclass
//                  CTapeImageFile (tape image file) subclass
//                  CTextInputFile (unit record input file) subclass
//                  CTextOutputFile (unit record output file) subclass
//
//   COPYRIGHT (C) 2015-2020 BY SPARE TIME GIZMOS.  ALL RIGHTS RESERVED.
//
// LICENSE:
//    This file is part of the emulator library project.  EMULIB is free
// software; you may redistribute it and/or modify it under the terms of
// the GNU Affero General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any
// later version.
//
//    EMULIB is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License
// for more details.  You should have received a copy of the GNU Affero General
// Public License along with EMULIB.  If not, see http://www.gnu.org/licenses/.
//
// DESCRIPTION:
//   A CImageFile object encapsulates a disk or tape image on the host file
// system.  A CDiskImage file is a derived class that represents a fixed
// length sector, random access, block rewritable (i.e. exactly what you'd
// expect from a disk drive!) file.  And conversely, a CTapeImageFile is a
// derived class that represents a variable record length, sequential access,
// non-rewritable (just what you'd want for a tape drive) file.
//
// Bob Armstrong <bob@jfcl.com>   [8-OCT-2013]
//
// REVISION HISTORY:
// 20-MAY-15  RLA   New file.
//  8-DEC-15  RLA   Add CTextInputFile and CTextOutputFile.
// 22-FEB-16  RLA   Add fixed length line support to CText?????ImageFile.
// 28-FEB-17  RLA   Make 64 bit clean.
//  1-JUN-17  RLA   Linux port.
// 26-AUG-22  RLA   Clean up Linux/WIN32 conditionals.
//  7-MAR-24  RLA   Add SetSectorSize for CDiskImageFile
//                  Add CHS addressing for CDiskImageFile
//--
#pragma once
#include <string>               // C++ std::string class, et al ...
using std::string;              // ...

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

class CImageFile {
  //++
  //   CImageFile is the base class for all disk and tape image files.  Notice
  // that this is an abstract class and it's not intended that anybody would
  // ever instantiate it directly.  There wouldn't be much point - it contains
  // no actual I/O methods!  All of those come from the derived disk or tape
  // classes.
  //--

  // Constants ...
public:
  //   These constants define platform independent names for shared file access.
  // On Windows we can just use the real constants for _fsopen(), but on Linux
  // we need to make up our own.  It's important that none of these values be
  // zero, even on Linux, so that we can distinguish the default argument value
  // in calls to Open().  Also notice that Linux has fewer options (at least if
  // you're using the flock() call) - we either lock the file or we don't.  In
  // that case the shared write and shared read options are the same!
  enum {
#if defined(_WIN32)
    SHARE_NONE  = _SH_DENYRW,    // no sharing (i.e. exclusive access)
    SHARE_READ  = _SH_DENYWR,    // shared reading, no writing
    SHARE_WRITE = _SH_DENYNO     // shared reading AND writing
#elif defined(__linux__)
    SHARE_NONE  = -1,           // no sharing (i.e. exclusive access)
    SHARE_WRITE =  1,           // or sharing (shared read and write!)
    SHARE_READ  = SHARE_WRITE   // there's no other option!
#elif defined(__APPLE__)
    SHARE_NONE  = 0,            // no sharing (i.e. exclusive access)
    SHARE_READ  = 1,            // shared reading
    SHARE_WRITE = 2             // shared writing
#elif defined(__unix__)
    SHARE_NONE  = 0,            // no sharing (i.e. exclusive access)
    SHARE_READ  = 1,            // shared reading
    SHARE_WRITE = 2             // shared writing
#endif
  };

public:
  // Constructor and destructor ...
  CImageFile();
  virtual ~CImageFile();
  // Disallow copy and assignment operations with CImageFile objects...
private:
  // Following lines give an error on Visual C++
  //CImageFile(const CImageFile &f) = delete;
  //CImageFile& operator= (const CImageFile &f) = delete;

  // Public methods ...
public:
    // Open or close the associated disk file ...
    virtual bool Open (const string &sFileName, bool fReadOnly=false, int nShareMode=0);
    virtual void Close();
    // Return various attributes (open, file name, read only, etc) ...
    bool IsOpen() const {return m_pFile != NULL;}
    bool IsReadOnly() const {return m_fReadOnly;}
    string GetFileName() const {return m_sFileName;}
    // Return true if we've hit the end of file ...
    bool IsEOF() const {return feof(m_pFile) != 0;}
    // Get the current file size or relative position (in bytes!) ...
    uint32_t GetFileLength() const;
    uint32_t GetFilePosition() const;
    // Set the file size, and truncate if necessary!
    bool SetFileLength (uint32_t nNewLength);
    // Truncate the file to the current position ...
    bool Truncate();

  // Local methods ...
protected:
  // Print a file related error message ...
  bool Error (const char *pszMsg, int nError) const;
  // Open the file and (on Linux) apply the file locking semantics ...
  bool TryOpenAndLock(const char *pszMode, int nShare);

  // Local members ...
protected:
  string   m_sFileName;   // name of the disk image file
  FILE    *m_pFile;       // handle of the image file
  bool     m_fReadOnly;   // TRUE if this image is R/O
  int      m_nShareMode;  // sharing mode for this file
};


class CDiskImageFile : public CImageFile {
  //++
  //   CDiskImageFile is the derived class for disk image files.  Disk images
  // have fixed length block/sector sizes, are block/sector rewritable, and
  // are random access.
  //--

public:
  // Constructor and destructor ...
  CDiskImageFile (uint32_t lSectorSize, uint16_t nCylinders=0, uint16_t nHeads=0, uint16_t nSectors=0);
  virtual ~CDiskImageFile() {};
  // Disallow copy and assignment operations with CDiskImageFile objects...
private:
  // Following lines give an error on Visual C++
  //CDiskImageFile (const CDiskImageFile &f) = delete;
  //CDiskImageFile& operator= (const CDiskImageFile &f) = delete;

  // Public methods ...
public:
  // Return the sector size.
  inline uint32_t GetSectorSize() const {return m_lSectorSize;}
  //   Set the sector size.  Note that we also reset the capacity to zero;
  // this will force it to be recalculated the next time GetCapacity()
  // is called...
  void SetSectorSize (uint32_t lSectorSize)
    {m_lSectorSize = lSectorSize;  m_lCapacity = 0;}
  // Set disk geometry ...
  void SetSectors   (uint16_t nSectors)   {m_nSectors = nSectors;}
  void SetHeads     (uint16_t nHeads)     {m_nHeads = nHeads;}
  void SetCylinders (uint16_t nCylinders) {m_nCylinders = nCylinders;}
  // Return geometry information ...
  inline uint16_t GetSectors()    const {return m_nSectors;   }
  inline uint16_t GetHeads()      const {return m_nHeads;     }
  inline uint16_t GetCylinders()  const {return m_nCylinders; }
  // Test C/H/S addresses for validity ...
  bool IsValidCylinder (uint16_t nCylinder) const {return nCylinder < GetCylinders();}
  bool IsValidHead (uint16_t nHead) const {return nHead < GetHeads();}
  bool IsValidSector (uint16_t nSector) const
    {return (nSector > 0) && (nSector <= GetSectors());}
  // Convert a C/H/S address to an absolute sector number ...
  enum {INVALID_SECTOR = 0xFFFFFFFFUL};
  bool IsValidCHS (uint16_t nCylinder, uint16_t nHead, uint16_t nSector) const;
  uint32_t CHStoLBA (uint16_t nCylinder, uint16_t nHead, uint16_t nSector) const;

  // Read or write sectors ...
  bool ReadSector (uint32_t lLBA, void *pData);
  bool ReadSector (uint16_t nCylinder, uint16_t nHead, uint16_t nSector, void *pData)
    {return ReadSector(CHStoLBA(nCylinder, nHead, nSector), pData);}
  bool WriteSector (uint32_t lLBA, const void *pData);
  bool WriteSector (uint16_t nCylinder, uint16_t nHead, uint16_t nSector, void *pData)
    {return WriteSector(CHStoLBA(nCylinder, nHead, nSector), pData);}
  // Get or change the disk capacity (in sectors) ...
  uint32_t GetCapacity() const;
  bool SetCapacity (uint32_t lCapacity, bool fTruncate=false);
  //   Return the capacity calculated from the CHS information only.  If the
  // CHS details are not set, this will return zero.
  uint32_t GetCHScapacity() const {return (uint32_t) m_nCylinders*m_nHeads*m_nSectors;}

  // Local methods ...
protected:
  // Seek to a particular sector ...
  bool SeekSector (uint32_t lLBA);

  // Local members ...
protected:
  uint32_t m_lSectorSize;       // disk sector/block size
  uint16_t m_nSectors;          // sectors per track
  uint16_t m_nHeads;            // surfaces (heads) per cylinder
  uint16_t m_nCylinders;        // cylinders per drive
  uint32_t m_lCapacity;         // disk capacity (in sectors!)
};
