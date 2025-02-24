//++
// ImageFile.cpp -> CImageFile (image file I/O for emulatir library) methods
//                  CDiskImageFile (disk image file) methods
//                  CTapeImageFile (tape image file) methods
//                  CTextInputFile (unit record input file) methods
//                  CTextOutputFile (unit record output file) methods
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
//   These class contains image file access methods.  A CImageFile object
// encapsulates any image file on the host file system.  A CDiskImage file is
// a derived class that represents a fixed length sector, random access, block
// rewritable (i.e. exactly what you'd expect from a disk drive!) file.  And
// conversely, a CTapeImageFile is a derived class that represents a variable
// record length, sequential access, non-rewritable (just what you'd want for
// a tape drive) file.  Lastly, CTextInputFile and CTextOutputFile
// are sequential text (i.e. translated ASCII) image file subclasses for unit
// record devices such as printers, card readers and card punches, etc.
//
//   The disk image file format is just a simple linear array of sectors stored
// verbatim, in ascending LBA order, and without any extra header words or
// other overhead.  It's the format used by simh and most other emulators. Note
// that the emulator library version here only deals with bytes. For disks that
// work with other word sizes the class that wraps this one must provide a
// conversion.  Lastly, notice that the disk sector size, ALWAYS in bytes, MUST
// be specified to the CDiskImage constructor - that's required by SeekSector()
// to calculate the correct offset.
// 
//    CDiskImageFile objects support either LBA or CHS (cylinder/head/sector)
// addressing.  For CHS addressing you must call SetGeometry() to define the
// physical disk geometry before any read or write operations.  SetGeometry()
// is required only for the CalculateLBA() method, and the ReadSector() and
// WRiteSector() versions that use CHS addressing.  Geometry does is NOT
// required for disks thst use LBA addressing only.  The sector size, however,
// must be set in all cases.
//
//   The tape image format is identical to the simh TAP format, with a single
// 32 bit header record stored at the start and end of each logical record.
// Nine track tape images are always stored as eight bit bytes - the ninth bit
// is a parity check and is not stored in the TAP file.  Packing and unpacking
// these bytes into whatever wordsize the host supports is always the problem
// of the controller - most machines, like CDC and DEC, have various and creative
// ways of handling it.
//
//   Note that there isn't really a simh standard for seven track tape images,
// although one common usage is to always set the MSB to zero and then store
// seven track tape frames as eight bit bytes.  In that usage a seven track
// image and a nine track tape image are indistinguishable, which is probably
// OK for practical purposes.  There's no special code in this class to support
// seven track tapes, although if you handle them this way then you don't need
// any.
//
// IMPORTANT!
//   Even the casual observer will notice that the read and write functions
// are NOT ENDIAN INDEPENDENT.  Right now that's a moot point because the
// only platform we support is a PC, and PCs are all little endian.
//
// Bob Armstrong <bob@jfcl.com>   [8-OCT-2013]
//
// REVISION HISTORY:
// 20-MAY-15  RLA   New file.
//  8-DEC-15  RLA   Add CTextInputFile and CTextOutputFile.
// 22-FEB-16  RLA   Add fixed length record support to CText????ImageFile.
//  8-MAR-16  RLA   Fix the mixed read/write bug with CTapeImageFiles.
// 28-FEB-17  RLA   Make 64 bit clean.
//  1-JUN-17  RLA   Linux port.
// 28-Sep-17  RLA   In ReadForwardRecord() don't die if we try to read at EOT.
// 22-JUL-22  RLA   Add SetCapacity() for disk image files ...
// 26-AUG-22  RLA   Clean up Linux/WIN32 conditionals.
//  7-MAR-24  RLA   Add CHS addressing to CDiskImageFile.
//--
//000000001111111111222222222233333333334444444444555555555566666666667777777777
//234567890123456789012345678901234567890123456789012345678901234567890123456789
#include <stdlib.h>             // exit(), system(), etc ...
//#include <stdint.h>	        // uint8_t, uint32_t, etc ...
#include <assert.h>             // assert() (what else??)
#include <errno.h>              // ENOENT, EACCESS, etc ...
#include <string.h>             // strcpy(), strerror(), etc ...
#include <cstring>              // needed for memset()
#if defined(_WIN32)
#include <io.h>                 // _chsize(), _fileno(), etc...
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#include <unistd.h>             // ftruncate(), etc ...
#include <sys/stat.h>           // needed for fstat() (what else??)
#include <sys/file.h>           // flock(), LOCK_EX, LOCK_SH, et al ...
#endif
#include "ImageFile.hpp"        // declarations for this module



///////////////////////////////////////////////////////////////////////////////
//  CImageFile members ...
///////////////////////////////////////////////////////////////////////////////

CImageFile::CImageFile()
{
  //++
  //   The constructor just initializes all the members - it DOES NOT open
  // any external disk file!  You'll have to call the Open() method to do that ...
  //--
  m_sFileName.clear();  m_pFile = NULL;  m_fReadOnly = false;
  m_nShareMode = SHARE_READ;
}

CImageFile::~CImageFile()
{
  //++
  // Close the disk file (if any) and destroy the CImage object ...
  //--
  Close();
}

bool CImageFile::Error (const char *pszMsg, int nError) const
{
  //++
  //   Print a file related error message and then always return false.  Why
  // always return false?  So we can say something like
  //
  //    if ... return Error("fail", errno);
  //--
  char sz[80];
  if (nError > 0) {
//    LOGS(ERROR_LVL, "error (" << nError << ") " << pszMsg << " " << m_sFileName);
//***    strerror_s(sz, sizeof(sz), nError);
//    LOGS(ERROR_LVL, sz);
  } else {
//    LOGS(ERROR_LVL, pszMsg << " - " << m_sFileName);
  }
  return false;
}

bool CImageFile::TryOpenAndLock (const char *pszMode, int nShare)
{
  //++
  //   This routine will try to open the file using the specified mode and, if
  // it is successful it will lock the file as required.  On Windows this can be
  // done all in one step with _fsopen(), but under Linux it takes two steps.
  // First, open the file and, if that works, call flock() to lock the file.
  // On both platforms the file name and file sharing mode are assumed to be
  // already set up in m_FileName and m_nShareMode.
  //
  //   This method will use the Linux flock() call to apply the desired file
  // lock, as indicated by m_nShareMode, to this file.  Under Linux, flock()
  // really only gives us two options - lock the file for exclusive access, or
  // lock the file for shared access (which really just prevents anyone else
  // from opening it for exclusive access).  There is no concept of shared
  // read-only access - you either share the file or you don't!
  //--
#if defined(_WIN32)
  m_pFile = _fsopen(m_sFileName.c_str(), pszMode, nShare);
  return (m_pFile != NULL);
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
  m_pFile = fopen(m_sFileName.c_str(), pszMode);
  if (m_pFile == NULL) return false;
  int lock = (nShare == SHARE_NONE) ? LOCK_EX : LOCK_SH;
  return (flock(fileno(m_pFile), lock|LOCK_NB) == 0);
#endif
}

bool CImageFile::Open (const string &sFileName, bool fReadOnly, int nShareMode)
{
  //++
  //   This method opens an image file associated with this object.  Simple,
  // but it has a couple of special cases.  First, if the fReadOnly flag is
  // true then the file is opened for read access only.  if fReadOnly is
  // false we normally try to open the file as read/write, BUT if the actual
  // image file has the read only attribute in the host file system then
  // we'll force read only mode.  This particular situation doesn't report any
  // error, but the caller can find out if it happened thru the IsReadOnly()
  // attribute of this object.
  //
  //   Also, if the file doesn't exist then we'll attempt to create an empty
  // file UNLESS the fReadOnly flag is true - in that case a non-existent file
  // is an error.  If the file can't be opened or created one way or another,
  // then an error message is printed, false is returned and this object
  // remains unopened.
  //
  //   Lastly, nShareMode can optionally specify the desired file sharing mode.
  // This defaults to SHARE_READ (i.e. shared reading, but only a single writer)
  // if not specified.  Note that the CImageFile class currently doesn't contain
  // any code for synchronizing access to a shared write file, so use this at
  // your own risk!
  //--
  assert(!sFileName.empty());
  if (nShareMode == 0)  nShareMode = fReadOnly ? SHARE_READ : SHARE_NONE;
  m_sFileName = sFileName;  m_fReadOnly = fReadOnly;  m_nShareMode = nShareMode;

  if (m_fReadOnly) {
    //   Open the file as read only.  In this case if it doesn't already exist,
    // there's not much we can do about it ...
    if (TryOpenAndLock("rb", m_nShareMode)) return true;
    return Error("opening", errno);
  }

  // Try to open the file for R/W access.  If we succeed, then quit now...
  if (TryOpenAndLock("rb+", m_nShareMode)) return true;

  //   If we failed because the file is read only, then switch this unit to
  // read only and attempt to continue ...
  if ((errno == EROFS) || (errno == EACCES)) {
    if (TryOpenAndLock("rb", SHARE_READ)) {
//      LOGS(DEBUG_LVL, "opening " << m_sFileName << " as read only");
      m_fReadOnly = true;  m_nShareMode = SHARE_READ;  return true;
    }
    return Error("opening", errno);
  }

  // If we failed because the file doesn't exist, then try to create it ...
  if (errno == ENOENT) {
    if (TryOpenAndLock("wb+", m_nShareMode)) {
//      LOGS(DEBUG_LVL, "creating empty file for " << m_sFileName);
      return true;
    }
    return Error("creating", errno);
  }

  // Otherwise we don't know what's wrong, so give up ...
  return Error("opening", errno);
}

void CImageFile::Close ()
{
  //++
  //   This routine closes an image file.  It's not very complicated, but
  // we'll spell it out for completeness...
  //--
  if (IsOpen()) {
#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    flock(fileno(m_pFile), LOCK_UN);
#endif
    fclose(m_pFile);
  }
  m_sFileName.clear();  m_pFile = NULL;
}

uint32_t CImageFile::GetFileLength() const
{
  //++
  // Get the current file size (in bytes!) ...
  //--
  assert(IsOpen());
#if defined(_WIN32)
  return _filelength(_fileno(m_pFile));
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
  struct stat st;
  if (fstat(fileno(m_pFile), &st) == 0) return st.st_size;
  Error("fstat", errno);  return 0;
#endif
}

uint32_t CImageFile::GetFilePosition() const
{
  //++
  // Get the current file position (in bytes!) ...
  //--
  assert(IsOpen());
  return ftell(m_pFile);
}

bool CImageFile::SetFileLength (uint32_t nNewLength)
{
  //++
  //   This method will change the length of this file.  If the new length
  // is longer than the current length, then the file is extended.  If the
  // new length is shorter (down to and including 0 bytes!), then the file
  // will be truncated ...
  //--
  if (IsReadOnly()) return false;
#if defined(_WIN32)
  if (_chsize(_fileno(m_pFile), nNewLength) == 0) return true;
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
  if (ftruncate(fileno(m_pFile), nNewLength) == 0) return true;
#endif
  return Error("change size", errno);
}

bool CImageFile::Truncate ()
{
  //++
  // Truncate the image file to the current position.
  //--
  assert(IsOpen());
  if (IsReadOnly()) return false;
  fseek(m_pFile, 0L, SEEK_CUR);
  return SetFileLength(ftell(m_pFile));
}



///////////////////////////////////////////////////////////////////////////////
// CDiskImageFile members ...
///////////////////////////////////////////////////////////////////////////////

CDiskImageFile::CDiskImageFile (uint32_t lSectorSize, uint16_t nCylinders, uint16_t nHeads, uint16_t nSectors)
{
  //++
  //   Initialize any disk image specific flags.  Note that the sector size
  // parameter is REQUIRED - we need it to calculate the correct disk offset
  // in SeekSector()!
  //--
  assert(lSectorSize > 0);
  m_lSectorSize = lSectorSize;  m_lCapacity = 0;
  m_nCylinders = nCylinders;  m_nHeads = nHeads;  m_nSectors = nSectors;
}

bool CDiskImageFile::IsValidCHS (uint16_t nCylinder, uint16_t nHead, uint16_t nSector) const
{
  //++
  // Return true if the C/H/S address is valid ...
  //--
  return    IsValidCylinder(nCylinder)
    && IsValidHead(nHead)
    && IsValidSector(nSector);
}

uint32_t CDiskImageFile::CHStoLBA (uint16_t nCylinder, uint16_t nHead, uint16_t nSector) const
{
  //++
  // Convert a C/H/S address to an absolute sector number ...
  //--
  if (IsValidCHS(nCylinder, nHead, nSector))
    return ((nCylinder * GetHeads()) + nHead) * GetSectors() + nSector-1;
  else
    return INVALID_SECTOR;
}

bool CDiskImageFile::SeekSector (uint32_t lLBA)
{
  //++
  //   This method will do an fseek() on the image file to move to the correct
  // offset for the specified absolute sector.  The sector size is used to
  // calculate the correct byte offset.
  //
  // Note that we use the 32 bit fseek() here rather than the 64 bit fseek64().
  // That's not really a problem at the moment, since the biggest disk we can
  // emulate only holds about 300Mb.
  //--
  assert(IsOpen());
  if (lLBA >= GetCapacity())
    return Error("bad LBA", 0);
  if (fseek(m_pFile, (lLBA * m_lSectorSize), SEEK_SET) != 0)
    return Error("seeking", errno);
  return true;
}

bool CDiskImageFile::ReadSector (uint32_t lLBA, void *pData)
{
  //++
  //   This method will read a single sector from a disk image file. Note that
  // the sector size, and therefore the number of bytes read, is fixed.  The
  // caller's buffer had better be big enough, or Bad Things will result!
  //
  //   Remember that new disk image files are NOT pre-allocated, so if the host
  // attempts to read a sector that's never been written before we'll end up
  // reading past the EOF.  That's not a problem, although it's not absolutely
  // clear what should happen in that case.  This routine always returns a
  // buffer of zeros for uninitialized disk data.
  //--
  assert(IsOpen());
  if (!SeekSector(lLBA)) return false;
  size_t count = fread(pData, 1, m_lSectorSize, m_pFile);
  if (count == 0) {
    // We're attempting to read past the EOF ...
    memset(pData, 0, m_lSectorSize);
  } else if (count != m_lSectorSize) {
    // Some kind of real file error occurred ...
    return Error("reading", errno);
  }
  return true;
}

bool CDiskImageFile::WriteSector (uint32_t lLBA, const void *pData)
{
  //++
  // Write a single sector to the image file ...
  //--
  assert(IsOpen());
  if (IsReadOnly()) return false;
  if (!SeekSector(lLBA)) return false;
  if (fwrite(pData, 1, m_lSectorSize, m_pFile) != m_lSectorSize)
    return Error("writing", errno);
  return true;
}

uint32_t CDiskImageFile::GetCapacity() const
{
  //++
  //   This routine will return the current capacity of the disk image, in
  // sectors or blocks.  It's one more than the maximum LBA that can be passed
  // to ReadSector() or WriteSector().  Note that this gets called fairly often
  // to range check the LBA, so we try to cache the result...
  // 
  //   Note that if CHS addressing is defined (i.e. nCylinders, nHeads and
  // nSectors are not zero) then we'll return a calculated value based entirely
  // on the defined geometry.  If CHS addressing is NOT defined, then we'll
  // return a value based on the actual file size.  It's this latter one that's
  // especially slow.
  //--
  if (m_lCapacity != 0) return m_lCapacity;
  if ((m_nCylinders != 0) && (m_nHeads != 0) && (m_nSectors != 0))
    const_cast<CDiskImageFile *>(this)->m_lCapacity = GetCHScapacity();
  else
    const_cast<CDiskImageFile *>(this)->m_lCapacity = GetFileLength() / m_lSectorSize;
  return m_lCapacity;
}

bool CDiskImageFile::SetCapacity (uint32_t lCapacity, bool fTruncate)
{
  //++
  //   And this routine will attempt to set the size of the disk image.  If
  // the new size is larger than the current size then the image will be
  // extended, however if the new size is smaller then nothing will happen
  // unless fTruncate is also true.  In that case, and only then, will data
  // be truncated off the end of the image.
  //--
  if ((lCapacity < GetCapacity())  &&  !fTruncate) return false;
  if (!SetFileLength(lCapacity * m_lSectorSize)) return false;
  m_lCapacity = lCapacity;
  return true;
}
