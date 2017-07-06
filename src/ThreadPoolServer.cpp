#include <fstream>
#include <iomanip>
#include "ThreadPoolServer.h"
#include "ThreadSafeStreamMap.h"

void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue<DataTypes::Packet>& queue, const std::string instrument, ThreadSafeStreamMap& outfiles)
{
    while(true)
    {
        DataTypes::Packet pack;

        if (queue.listen(pack))
        {
            if (pack.ignored)
                continue;
            std::ofstream& outfile = outfiles.getStream(instrument, pack.apid);
            if (static_cast<uint64_t>(outfile.tellp()) == 0)
            {
                outfile << std::setw(15) << "Day" << "," << std::setw(15) << "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
                for (const DataTypes::Numeric& num : pack.data)
                {
                    outfile << std::setw(15) << num.mnem << ",";
                }
                outfile << "\n";
            }

            outfile << std::setw(15) << pack.day << "," << std::setw(15) << pack.millis << "," << std::setw(15) << pack.micros << "," << std::setw(15) << pack.sequenceCount << ",";
            for (const DataTypes::Numeric& num : pack.data)
            {
                switch (num.tag)
                {
                case DataTypes::Numeric::I32: outfile << std::setw(15) << std::right << num.i32; break;
                case DataTypes::Numeric::U32: outfile << std::setw(15) << std::right << num.u32; break;
                case DataTypes::Numeric::F64: outfile << std::setw(15) << std::right << num.f64; break;
                }
                outfile << ",";
            }
            outfile << "\n";
        }
        else
            return;
    }
}
void ThreadPoolServer::start()
{
    m_thread = std::thread(&ThreadPoolServer::ThreadMain, this, std::ref(m_queue), std::ref(m_instrument), std::ref(m_outfiles));
}

void ThreadPoolServer::exec(const DataTypes::Packet pack)
{
    m_queue.push(pack);
}

void ThreadPoolServer::join()
{
    std::cout << std::endl << "Waiting for writer threads to finish...";
    m_thread.join();
    for(auto& stream: m_outfiles)
        stream.second.close();
}
