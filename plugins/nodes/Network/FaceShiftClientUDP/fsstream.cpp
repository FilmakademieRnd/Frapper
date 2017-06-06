#include "fsstream.h"

#define FSNETWORKMAGICNUMBER 33433
#define FSNETWORKVERSION 1

#define BLOCKID_INFO            101
#define BLOCKID_POSE            102
#define BLOCKID_BLENDSHAPES     103
#define BLOCKID_EYES            104
#define BLOCKID_MARKERS         105

namespace fs {

bool fsTrackingData::operator==(const fsTrackingData & other)
{
    if( fabs( m_timestamp - other.m_timestamp ) > 1e-1 ) { printf("timestamp different %f %f\n", m_timestamp, other.m_timestamp ); return false; }
    if( m_trackingSuccessful != other.m_trackingSuccessful ) { printf("success different\n"); return false; }
    if( (m_headRotation - other.m_headRotation).Norm() > 1e-6 ) { printf("rotation different %f\n",(m_headRotation - other.m_headRotation).Norm()); return false; }
    if( (m_headTranslation-other.m_headTranslation).length() > 1e-3 ) { printf("translation different %f\n", (m_headTranslation-other.m_headTranslation).length()); return false; }
    if( fabs( m_eyeGazeLeftPitch-other.m_eyeGazeLeftPitch) > 1e-3 ) { printf("gaze left pitch different\n"); return false; }
    if( fabs( m_eyeGazeLeftYaw-other.m_eyeGazeLeftYaw) > 1e-3 ) { printf("gaze left yaw different\n"); return false; }
    if( fabs( m_eyeGazeRightPitch-other.m_eyeGazeRightPitch) > 1e-3 ) { printf("gaze right pitch different\n"); return false; }
    if( fabs( m_eyeGazeRightYaw-other.m_eyeGazeRightYaw) > 1e-3 ) { printf("gaze right yaw different\n"); return false; }
    if( m_coeffs.size() != other.m_coeffs.size() ) { printf("num coeffs different\n"); return false; }
    for(int i = 0;i < int(m_coeffs.size()); i++) if( fabs( m_coeffs[i]-other.m_coeffs[i]) > 1e-6 ) { printf("coeffs different\n"); return false; }
    if( m_markers.size() != other.m_markers.size() ) { printf("num markers different\n"); return false; }
    for(int i = 0;i < int(m_markers.size()); i++) if( (m_markers[i]-other.m_markers[i]).length() > 1e-3 ) { printf("markers different %f\n", (m_markers[i]-other.m_markers[i]).length()); return false; }

    return true;
}

fsDataBlock::fsDataBlock()
{

}

fsDataBlock::fsDataBlock( quint16 blockID, quint16 version, const QByteArray & data ):
    m_blockID(blockID),
    m_version(version),
    m_data(data)
{

}

//! returns whether @param data contains a block header
bool fsDataBlock::headerAvailable( const QByteArray & data )
{
    return (  data.size() >= int( 2*sizeof(quint16)+sizeof(quint32)) );
}


//! returns whether @param data contains a full block
bool fsDataBlock::blockAvailable( const QByteArray & data )
{
    fsDataStream stream(data);
    quint16 blockID = 0;
    quint16 version = 0;
    quint32 blockSize = 0;
    stream >> blockID >> version >> blockSize;
    return( data.size() >= int(2*sizeof(quint16)+sizeof(quint32)+blockSize) );
}

//! return next block id
quint16 fsDataBlock::nextBlockID( const QByteArray & data )
{
    if( !headerAvailable(data) ) return 0;
    fsDataStream stream(data);
    quint16 blockID = 0;
    stream >> blockID;
    return blockID;
}

QByteArray fsDataBlock::encode()
{
    QByteArray data;
    fsDataStream stream(&data,QIODevice::WriteOnly);

    stream << quint16(m_blockID);
    stream << quint16(m_version);
    stream << quint32(m_data.size());
    stream.writeRawData( m_data.data(), m_data.size() );

    return data;
}

bool fsDataBlock::decode( const QByteArray & data )
{
    if(!headerAvailable(data)) return false;
    if(!blockAvailable(data)) return false;

    fsDataStream stream(data);
    quint16 blockID = 0;
    quint16 version = 0;
    quint32 blockSize =0;
    stream >> blockID >> version >> blockSize;

    // sanity check of data
    if( blockSize > 1e10 ) return false;

    QByteArray blockData = stream.device()->read( blockSize );
    bool success = ( quint32(blockData.size()) == blockSize );

    if(success) {
        m_blockID = blockID;
        m_version = version;
        m_data = blockData;
    }

    return success;
}


QByteArray fsBinaryTrackingStream::encodeInfo(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsDataStream stream(&data, QIODevice::WriteOnly);
    stream << _trackingData.m_timestamp << quint8( _trackingData.m_trackingSuccessful );
    return fs::fsDataBlock( BLOCKID_INFO, 1.0, data ).encode();
}


QByteArray fsBinaryTrackingStream::encodePose(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsDataStream stream(&data, QIODevice::WriteOnly);
    stream << _trackingData.m_headRotation.x << _trackingData.m_headRotation.y << _trackingData.m_headRotation.z << _trackingData.m_headRotation.w;
    stream << _trackingData.m_headTranslation.x << _trackingData.m_headTranslation.y << _trackingData.m_headTranslation.z;
    return fs::fsDataBlock( BLOCKID_POSE, 1.0, data ).encode();
}

QByteArray fsBinaryTrackingStream::encodeBlendshapes(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsDataStream stream(&data, QIODevice::WriteOnly);
    quint32 numParameters = _trackingData.m_coeffs.size();
    stream << numParameters;
    for(quint32 k = 0; k < numParameters; k++) stream << _trackingData.m_coeffs[k];
    return fs::fsDataBlock( BLOCKID_BLENDSHAPES, 1.0, data ).encode();
}

QByteArray fsBinaryTrackingStream::encodeEyeGaze(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsDataStream stream(&data, QIODevice::WriteOnly);
    stream << _trackingData.m_eyeGazeLeftPitch;
    stream << _trackingData.m_eyeGazeLeftYaw;
    stream << _trackingData.m_eyeGazeRightPitch;
    stream << _trackingData.m_eyeGazeRightYaw;
    return fs::fsDataBlock( BLOCKID_EYES, 1.0, data ).encode();
}

QByteArray fsBinaryTrackingStream::encodeMarkers(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsDataStream stream(&data, QIODevice::WriteOnly);
    stream << quint16( _trackingData.m_markers.size() );
    for(unsigned i = 0; i < _trackingData.m_markers.size(); i++)
    {
        stream << _trackingData.m_markers[i].x << _trackingData.m_markers[i].y << _trackingData.m_markers[i].z;
    }
    return fs::fsDataBlock( BLOCKID_MARKERS, 1.0, data ).encode();
}


void fsBinaryTrackingStream::decodeInfo(fsTrackingData & _trackingData,const QByteArray & data)
{
    fs::fsDataStream stream(data);
    stream >> _trackingData.m_timestamp;
    quint8 trackingSuccessful = 0;
    stream >> trackingSuccessful;
    _trackingData.m_trackingSuccessful = trackingSuccessful;
}

void fsBinaryTrackingStream::decodePose(fsTrackingData & _trackingData,const QByteArray & data)
{
    fs::fsDataStream stream(data);
    float x,y,z,w;
    stream >> x >> y >> z >> w;
    _trackingData.m_headRotation = Ogre::Quaternion(w,x,y,z);
    stream >> _trackingData.m_headTranslation.x >> _trackingData.m_headTranslation.y >> _trackingData.m_headTranslation.z;
}

void fsBinaryTrackingStream::decodeBlendshapes(fsTrackingData & _trackingData,const QByteArray & data)
{
    fs::fsDataStream stream(data);
     quint32 numParameters = 0;
     stream >> numParameters;
     _trackingData.m_coeffs.resize(numParameters);
     for(quint32 i = 0; i < numParameters; i++) stream >> _trackingData.m_coeffs[i];
}

void fsBinaryTrackingStream::decodeEyeGaze(fsTrackingData & _trackingData,const QByteArray & data)
{
    fs::fsDataStream stream(data);
    stream >> _trackingData.m_eyeGazeLeftPitch;
    stream >> _trackingData.m_eyeGazeLeftYaw;
    stream >> _trackingData.m_eyeGazeRightPitch;
    stream >> _trackingData.m_eyeGazeRightYaw;
}

void fsBinaryTrackingStream::decodeMarkers(fsTrackingData & _trackingData,const QByteArray & data)
{
    fs::fsDataStream stream(data);
    quint16 numMarkers = 0;
    stream >> numMarkers;
    _trackingData.m_markers.resize(numMarkers);
    for(int i = 0; i < numMarkers; i++)
    {
        stream >> _trackingData.m_markers[i].x >> _trackingData.m_markers[i].y >> _trackingData.m_markers[i].z;
    }
}



//! serialize tracking state & markers
void fsBinaryTrackingStream::encode(const fsTrackingData & _trackingData)
{
    int numBlocks = 0;
    QByteArray allBlockData;

    // serialize all parts
    numBlocks++; allBlockData.append( encodeInfo(_trackingData) );
    numBlocks++; allBlockData.append( encodePose(_trackingData) );
    numBlocks++; allBlockData.append( encodeBlendshapes(_trackingData) );
    numBlocks++; allBlockData.append( encodeEyeGaze(_trackingData) );
    numBlocks++; allBlockData.append( encodeMarkers(_trackingData) );

    // combine all data into the fs container block
    QByteArray combinedBlockData;
    fs::fsDataStream stream(&combinedBlockData, QIODevice::WriteOnly);
    stream << quint16(numBlocks);
    combinedBlockData.append(allBlockData);
    fs::fsDataBlock containerBlock( FSNETWORKMAGICNUMBER, FSNETWORKVERSION, combinedBlockData);

    // append the block data to the full data
    m_data.append(containerBlock.encode());
}


//! decode the tracking data from the stream
bool fsBinaryTrackingStream::decode( fsTrackingData & _trackingData, bool _removeDataAfterDecoding )
{
    m_valid = true;

    if( !fsDataBlock::headerAvailable(m_data) ) { return false; }
    if( !fsDataBlock::blockAvailable(m_data) ) { return false; }

    fsDataBlock block;
    // decode the block
    if( !block.decode(m_data) ) { m_valid = false; return false; }
    // check for magic number
    if( block.m_blockID != FSNETWORKMAGICNUMBER ) { m_valid = false; return false; }

    // decode all blocks
    QByteArray combinedBlockData = block.m_data;
    fs::fsDataStream stream( combinedBlockData );
    // get number of blocks contained in the container
    quint16 numBlocks = 0;
    stream >> numBlocks;
    combinedBlockData.remove(0,sizeof(quint16) );
    for(int i = 0; i < numBlocks; i++)
    {
        // decode each block
        fsDataBlock block;
        if( !block.decode(combinedBlockData) ) { m_valid = false; return false; }
        combinedBlockData.remove(0, block.getTotalSize());

        switch(block.m_blockID)
        {
            default: m_valid = false; break;
            case BLOCKID_INFO: decodeInfo(_trackingData, block.m_data); break;
            case BLOCKID_POSE: decodePose(_trackingData, block.m_data); break;
            case BLOCKID_BLENDSHAPES: decodeBlendshapes(_trackingData, block.m_data); break;
            case BLOCKID_EYES: decodeEyeGaze(_trackingData, block.m_data); break;
            case BLOCKID_MARKERS: decodeMarkers(_trackingData, block.m_data); break;
        }
        if(!m_valid) return false;
    }

    if(_removeDataAfterDecoding)
    {
        // remove the full block
        m_data.remove(0, block.getTotalSize());
    }

    return true;
}





QByteArray fsAsciiTrackingStream::encodeInfo(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsTextStream stream(&data, QIODevice::WriteOnly);
    stream << "I " << _trackingData.m_timestamp << " " << quint8( _trackingData.m_trackingSuccessful ) << " ";
    return data;
}


QByteArray fsAsciiTrackingStream::encodePose(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsTextStream stream(&data, QIODevice::WriteOnly);
    stream << "P ";
    double rx = _trackingData.m_headRotation.x;
    double ry = _trackingData.m_headRotation.y;
    double rz = _trackingData.m_headRotation.z;
    double rw = _trackingData.m_headRotation.w;
    //printf("rotation: %f %f %f %f\n", rx,ry,rz,rw);
    stream << rx << " " << ry << " " << rz << " " << rw << " ";
    stream << _trackingData.m_headTranslation.x << " " << _trackingData.m_headTranslation.y << " " << _trackingData.m_headTranslation.z << " ";
    return data;
}

QByteArray fsAsciiTrackingStream::encodeBlendshapes(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsTextStream stream(&data, QIODevice::WriteOnly);
    stream << "C ";
    quint32 numParameters = _trackingData.m_coeffs.size();
    stream << numParameters << " ";
    for(quint32 k = 0; k < numParameters; k++) stream << _trackingData.m_coeffs[k] << " ";
    return data;
}

QByteArray fsAsciiTrackingStream::encodeEyeGaze(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsTextStream stream(&data, QIODevice::WriteOnly);
    stream << "E ";
    stream << _trackingData.m_eyeGazeLeftPitch << " ";
    stream << _trackingData.m_eyeGazeLeftYaw << " ";
    stream << _trackingData.m_eyeGazeRightPitch << " ";
    stream << _trackingData.m_eyeGazeRightYaw << " ";
    return data;
}

QByteArray fsAsciiTrackingStream::encodeMarkers(const fsTrackingData & _trackingData)
{
    QByteArray data;
    fs::fsTextStream stream(&data, QIODevice::WriteOnly);
    stream << "M ";
    stream << quint16( _trackingData.m_markers.size() ) << " ";
    for(unsigned i = 0; i < _trackingData.m_markers.size(); i++)
    {
        stream << _trackingData.m_markers[i].x << " " << _trackingData.m_markers[i].y << " " << _trackingData.m_markers[i].z << " ";
    }
    return data;
}


void fsAsciiTrackingStream::decodeInfo(fsTrackingData & _trackingData,fs::fsTextStream & stream)
{
    stream >> _trackingData.m_timestamp;
    int trackingSuccessful = 0;
    stream >> trackingSuccessful;
    _trackingData.m_trackingSuccessful = trackingSuccessful;
}

void fsAsciiTrackingStream::decodePose(fsTrackingData & _trackingData,fs::fsTextStream & stream)
{
    double x,y,z,w;
    stream >> x >> y >> z >> w;
    _trackingData.m_headRotation = Ogre::Quaternion(w,x,y,z);
    stream >> _trackingData.m_headTranslation.x >> _trackingData.m_headTranslation.y >> _trackingData.m_headTranslation.z;
}

void fsAsciiTrackingStream::decodeBlendshapes(fsTrackingData & _trackingData,fs::fsTextStream & stream)
{
     quint32 numParameters = 0;
     stream >> numParameters;
     _trackingData.m_coeffs.resize(numParameters);
     for(quint32 i = 0; i < numParameters; i++) stream >> _trackingData.m_coeffs[i];
}

void fsAsciiTrackingStream::decodeEyeGaze(fsTrackingData & _trackingData, fs::fsTextStream & stream )
{
    stream >> _trackingData.m_eyeGazeLeftPitch;
    stream >> _trackingData.m_eyeGazeLeftYaw;
    stream >> _trackingData.m_eyeGazeRightPitch;
    stream >> _trackingData.m_eyeGazeRightYaw;
}

void fsAsciiTrackingStream::decodeMarkers(fsTrackingData & _trackingData, fs::fsTextStream & stream)
{
    quint16 numMarkers = 0;
    stream >> numMarkers;
    _trackingData.m_markers.resize(numMarkers);
    for(int i = 0; i < numMarkers; i++)
    {
        stream >> _trackingData.m_markers[i].x >> _trackingData.m_markers[i].y >> _trackingData.m_markers[i].z;
    }
}



//! serialize tracking state & markers
void fsAsciiTrackingStream::encode(const fsTrackingData & _trackingData)
{
    int numBlocks = 0;
    QByteArray allBlockData;

    // serialize all parts
    numBlocks++; allBlockData.append( encodeInfo(_trackingData) );
    numBlocks++; allBlockData.append( encodePose(_trackingData) );
    numBlocks++; allBlockData.append( encodeBlendshapes(_trackingData) );
    numBlocks++; allBlockData.append( encodeEyeGaze(_trackingData) );
    numBlocks++; allBlockData.append( encodeMarkers(_trackingData) );

    // combine all data into the fs container block
    QByteArray combinedBlockData;
    fs::fsTextStream stream(&combinedBlockData, QIODevice::WriteOnly);
    stream << "FS " << quint16(numBlocks) << " ";
    stream.flush();
    combinedBlockData.append(allBlockData);
    combinedBlockData.append('\r');
    combinedBlockData.append('\n');

    m_data.append(combinedBlockData);
}


//! decode the tracking data from the stream
bool fsAsciiTrackingStream::decode( fsTrackingData & _trackingData, bool _removeDataAfterDecoding )
{
    m_valid = true;

    fs::fsTextStream stream(m_data);
    QString name;
    stream >> name;
    if( name != "FS" ) {
        if( !stream.atEnd() ) m_valid = false;
        return false;
    }

    // we read FS, so lets get the different blocks
    int numBlocks = 0;
    stream >> numBlocks;

    for(int i = 0; i < numBlocks; i++)
    {
        stream >> name;
        if(name == "I") decodeInfo(_trackingData,stream);
        else if(name == "P") decodePose(_trackingData,stream);
        else if(name == "C") decodeBlendshapes(_trackingData,stream);
        else if(name == "E") decodeEyeGaze(_trackingData,stream);
        else if(name == "M") decodeMarkers(_trackingData,stream);
        else {
            m_valid = false;
            return false;
        }
    }

    //TODO: add checks on stream to see whether it was successful

    if(_removeDataAfterDecoding)
    {
        // remove the full block
        m_data.remove(0, stream.pos());
    }

    return true;
}

}

