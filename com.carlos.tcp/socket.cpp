


std::ostringstream archive_stream;
boost::archive::text_oarchive archive(archive_stream);
archive << YOUR_DATA;
outbound_data_ = archive_stream.str();
boost::asio::async_write(socket_, boost::asio::buffer(outbound_data_), handler);