#include <fstream>
#include <iomanip>
#include <memory>
#include <iostream>
#include "ThreadPoolServer.h"
#include "ThreadSafeStreamMap.h"

/**
 * Writer thread's main loop. Thread waits until it can pull a packet from the queue and then writes the packet contents to an output file.
 *
 * @param queue ThreadSafeQueue containing parsed packets
 * @param instrument Instrument string for output file naming purposes
 * @param outfiles ThreadSafeStreamMap for accessing file output streams
 * @return N/A
 */
void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue& queue, const std::string instrument, ThreadSafeStreamMap& outfiles)
{
    while (true)  // Loop until return due to empty queue
    {
        uint32_t retVal = 0;
        auto queueVal = queue.listen(retVal);  // Listen on queue until queueVal is returned (tuple with lock and stream)
        if (retVal)  // retVal is 1 on success
        {
            if (std::get<0>(queueVal)->ignored)  // If packet is ignored, skip
            {
                std::get<1>(queueVal)->unlock();  // Unlock the mutex before skipping
                continue;
            }

            std::ofstream& outfile = outfiles.getStream(instrument, std::get<0>(queueVal)->apid);  // Get file stream from map

            if (static_cast<uint64_t>(outfile.tellp()) == 0)  // If at beginning of file then write column headers
            {
                outfile << std::setw(15) << "Day" << "," << std::setw(15) << "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
                for (const DataTypes::Numeric& num : std::get<0>(queueVal)->data)
                {
                    outfile << std::setw(15) << num.mnem << ",";
                }
                outfile << "\n";
            }

            outfile << std::setw(15) << std::get<0>(queueVal)->day << "," << std::setw(15) << std::get<0>(queueVal)->millis << "," << std::setw(15) << std::get<0>(queueVal)->micros << "," << std::setw(15) << std::get<0>(queueVal)->sequenceCount << ",";  // Write time and sequenceCount
            for (const DataTypes::Numeric& num : std::get<0>(queueVal)->data)  // Loop through packet data and write to file
            {
                switch (num.tag)  // Switch on data type
                {
                case DataTypes::Numeric::I32: outfile << std::setw(15) << std::right << num.i32; break;
                case DataTypes::Numeric::U32: outfile << std::setw(15) << std::right << num.u32; break;
                case DataTypes::Numeric::F64: outfile << std::setw(15) << std::right << num.f64; break;
                default: break;
                }
                outfile << ",";
            }
            outfile << "\n";
            std::get<1>(queueVal)->unlock();  // Unlock the mutex that was returned from the queue, because we are done writing
        }
        else
            return;  // If queue listen times out, then terminate thread
    }
}


/**
 * Starts writers threads and stores threads in member vector
 *
 * @return N/A
 */
void ThreadPoolServer::start()
{
    for (uint32_t i = 0; i < m_num_threads; ++i)
    {
        m_threads.emplace_back(std::thread(&ThreadPoolServer::ThreadMain, this, std::ref(m_queue), std::ref(m_instrument), std::ref(m_outfiles)));
    }
}

/**
 * Moves a Packet pointer into the queue
 *
 * @param pack unique_ptr pointing to packet
 * @return N/A
 */
void ThreadPoolServer::exec(std::unique_ptr<DataTypes::Packet> pack)
{
    m_queue.push(std::move(pack));
}

/**
 * Called by Decom once the input file is finished being parsed. Waits for writers threads to finish (Empty queue and timeout)
 * Also closes output files.
 *
 * @return N/A
 */
void ThreadPoolServer::join()
{
    std::cout << std::endl << "Waiting for writer threads to finish...";
    for (auto& thread : m_threads)
        thread.join();
    for (auto& stream : m_outfiles)
        stream.second.close();
}
