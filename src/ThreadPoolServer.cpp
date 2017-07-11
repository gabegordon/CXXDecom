#include <fstream>
#include <iomanip>
#include <memory>
#include <iostream>
#include "ThreadPoolServer.h"

/**
 * Writer thread's main loop. Thread waits until it can pull a packet from the queue and then writes the packet contents to an output file.
 *
 * @param queue ThreadSafeQueue containing parsed packets
 * @param instrument Instrument string for output file naming purposes
 * @param apid APID for output file naming purposes
 * @return N/A
 */
void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue& queue, const std::string& instrument, const uint32_t apid)
{
    std::ofstream outfile("output/" + instrument + "_" + std::to_string(apid) + ".txt", std::ios::ate);
    bool firstRun = true;

    while (true)  // Loop until return due to empty queue
    {
        uint32_t retVal = 0;
        auto queueVal = queue.listen(retVal);  // Listen on queue until queueVal is returned (tuple with lock and stream)
        if (retVal)  // retVal is 1 on success
        {
            if (firstRun)
            {
                outfile << std::setw(15) << "Day" << "," << std::setw(15) << "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
                for (const DataTypes::Numeric& num : queueVal->data)
                {
                    outfile << std::setw(15) << num.mnem << ",";
                }
                outfile << "\n";
                firstRun = false;
            }

            outfile << std::setw(15) << queueVal->day << "," << std::setw(15) << queueVal->millis << "," << std::setw(15) << queueVal->micros << "," << std::setw(15) << queueVal->sequenceCount << ",";  // Write time and sequenceCount
            for (const DataTypes::Numeric& num : queueVal->data)  // Loop through packet data and write to file
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
        }
        else
        {
            outfile.close();
            return;  // If queue listen times out, then terminate thread
        }
    }
}


/**
 * Moves a Packet pointer into the corresponding queue and creates new thread if we have a new APID
 *
 * @param pack unique_ptr pointing to packet
 * @return N/A
 */
void ThreadPoolServer::exec(std::unique_ptr<DataTypes::Packet> pack)
{
    if(pack->ignored)
    {
        return;
    }
    else if (m_queues.count(pack->apid))
    {
        m_queues[pack->apid].push(std::move(pack));
    }
    else
    {
        m_threads.emplace_back(std::thread(&ThreadPoolServer::ThreadMain, this, std::ref(m_queues[pack->apid]), std::ref(m_instrument), pack->apid));
        m_queues[pack->apid].push(std::move(pack));
    }
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
    for (auto& q : m_queues)
        q.second.setInactive();
    for (auto& thread : m_threads)
        thread.join();
}
