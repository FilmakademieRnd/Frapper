/*
-----------------------------------------------------------------------------
This source file is part of FRAPPER
research.animationsinstitut.de
sourceforge.net/projects/frapper

Copyright (c) 2008-2016 Filmakademie Baden-Wuerttemberg, Institute of Animation 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; version 2.1 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
-----------------------------------------------------------------------------
*/

#ifndef FSSTREAM_H
#define FSSTREAM_H

#include <QDataStream>
#include <QTextStream>
#include <QByteArray>
#include <vector>

#include <OgreQuaternion.h>
#include <OgreVector3.h>

namespace fs {

//! faceshift tracking data binary block
class fsTrackingData
{
public:
    //! time stamp in ms or s
    double m_timestamp;

    //! flag whether tracking was successful [0,1]
    bool m_trackingSuccessful;

    //! head pose
    Ogre::Quaternion m_headRotation;
    Ogre::Vector3 m_headTranslation;

    //! eye gaze in degrees
    float m_eyeGazeLeftPitch;
    float m_eyeGazeLeftYaw;
    float m_eyeGazeRightPitch;
    float m_eyeGazeRightYaw;

    //! blendshape coefficients
    std::vector<float> m_coeffs;

    //! marker positions - format specified in faceshift
    std::vector< Ogre::Vector3 > m_markers;

    //! (fuzzy) comparison between tracking data
    bool operator==(const fsTrackingData & other);

public:

};



//! data stream in correct binary format
class fsDataStream : public QDataStream
{
public:
    fsDataStream( QByteArray * data, QIODevice::OpenMode mode ):QDataStream(data,mode) {
        setVersion( QDataStream::Qt_4_5 );          // minimum  Qt_4_5 stream
        setByteOrder(QDataStream::LittleEndian);    // little endian encoding
    }
    fsDataStream( const QByteArray & data):QDataStream(data) {
        setVersion( QDataStream::Qt_4_5 );          // minimum  Qt_4_5 stream
        setByteOrder(QDataStream::LittleEndian);    // little endian encoding
    }
    virtual ~fsDataStream() {}
};

//! stream in text format
class fsTextStream : public QTextStream
{
public:
    fsTextStream( QByteArray * data, QIODevice::OpenMode mode ):QTextStream(data,mode){}
    fsTextStream( const QByteArray & data ):QTextStream(data){}
    virtual ~fsTextStream() {}
};


//! data binary block
class fsDataBlock
{
public:
    fsDataBlock();
    fsDataBlock( quint16 blockID, quint16 version, const QByteArray & data );

    //! write out the block
    QByteArray encode();

    //! decode the block from the @param data stream - note that @param data is not modified, so you need to remove the data block manually
    bool decode( const QByteArray & data );

    //! returns whether @param data contains a header
    static bool headerAvailable( const QByteArray & data );

    //! returns whether @param data contains a full block
    static bool blockAvailable( const QByteArray & data );

    //! return next block id in @param data
    static quint16 nextBlockID( const QByteArray & data );

    quint32 getTotalSize() { return m_data.size() + 2*sizeof(quint16) + sizeof(quint32); }
    quint32 getBlockSize() { return m_data.size(); }

    quint16 m_blockID;
    quint16 m_version;
    QByteArray m_data;
};


/**
  * class to read/write a faceshift binary tracking stream
  *
  * to read the tracking state from a binary stream do the following:
  * 1. append the network stream data
  * 2. call decode to get the tracking data
  * 3. if the decode call was successful you can access the data in fsTrackingData
  */
class fsBinaryTrackingStream {
public:
    //! clear the data
    void clear()  { m_data.clear(); }

    //! append tracking stream data
    void append(const QByteArray & data) { m_data.append( data ); }

    //! serialize tracking state & markers
    void encode(const fsTrackingData & _trackingData);

    /**
     * decode the tracking data from the stream
     * returns true if successful decoded data, returns false if not enough data available or if error (check for error using isValid())
     */
    bool decode( fsTrackingData & _trackingData, bool _removeDataAfterDecoding = true );

    //! return the serialized data
    const QByteArray & getData() { return m_data; }

    //! check whether input data is valid after unsuccessful decode
    bool isValid() const { return m_valid; }

private:
    //! encode different parts
    QByteArray encodeInfo(const fsTrackingData & _trackingData);
    QByteArray encodePose(const fsTrackingData & _trackingData);
    QByteArray encodeBlendshapes(const fsTrackingData & _trackingData);
    QByteArray encodeEyeGaze(const fsTrackingData & _trackingData);
    QByteArray encodeMarkers(const fsTrackingData & _trackingData);

    //! decode different parts
    void decodeInfo(fsTrackingData & _trackingData,const QByteArray & data);
    void decodePose(fsTrackingData & _trackingData,const QByteArray & data);
    void decodeBlendshapes(fsTrackingData & _trackingData,const QByteArray & data);
    void decodeEyeGaze(fsTrackingData & _trackingData,const QByteArray & data);
    void decodeMarkers(fsTrackingData & _trackingData,const QByteArray & data);

    //! the serialized data
    QByteArray m_data;

    //! flag whether input data is valid
    bool m_valid;
};



/**
  * class to read/write a faceshift ascii tracking stream
  *
  * to read the tracking state from an ascii stream do the following:
  * 1. append the network stream data
  * 2. call decode to get the tracking data
  * 3. if the decode call was successful you can access the data in fsTrackingData
  */
class fsAsciiTrackingStream {
public:
    //! clear the data
    void clear()  { m_data.clear(); }

    //! append tracking stream data
    void append(const QByteArray & data) { m_data.append( data ); }

    //! serialize tracking state & markers
    void encode(const fsTrackingData & _trackingData);

    /**
     * decode the tracking data from the stream
     * returns true if successful decoded data, returns false if not enough data available or if error (check for error using isValid())
     */
    bool decode( fsTrackingData & _trackingData, bool _removeDataAfterDecoding = true );

    //! return the serialized data
    const QByteArray & getData() { return m_data; }

    //! check whether input data is valid after unsuccessful decode
    bool isValid() const { return m_valid; }

private:
    //! encode different parts
    QByteArray encodeInfo(const fsTrackingData & _trackingData);
    QByteArray encodePose(const fsTrackingData & _trackingData);
    QByteArray encodeBlendshapes(const fsTrackingData & _trackingData);
    QByteArray encodeEyeGaze(const fsTrackingData & _trackingData);
    QByteArray encodeMarkers(const fsTrackingData & _trackingData);

    //! decode different parts
    void decodeInfo(fsTrackingData & _trackingData, fs::fsTextStream & stream);
    void decodePose(fsTrackingData & _trackingData, fs::fsTextStream & stream);
    void decodeBlendshapes(fsTrackingData & _trackingData, fs::fsTextStream & stream);
    void decodeEyeGaze(fsTrackingData & _trackingData, fs::fsTextStream & stream);
    void decodeMarkers(fsTrackingData & _trackingData, fs::fsTextStream & stream);

    //! the serialized data
    QByteArray m_data;

    //! flag whether input data is valid
    bool m_valid;
};

}

#endif // FSSTREAM_H

