#include <fstream>
#include <iomanip>
#include <memory>
#include "ThreadPoolServer.h"
#include "ThreadSafeStreamMap.h"

void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue& queue, const std::string instrument, ThreadSafeStreamMap& outfiles)
{
    while (true)
    {
        uint32_t retVal = 0;
        auto queueVal = queue.listen(retVal);
        if (retVal)
        {
            if (std::get<0>(queueVal)->ignored)
                continue;
            std::ofstream& outfile = outfiles.getStream(instrument, std::get<0>(queueVal)->apid);
            if (static_cast<uint64_t>(outfile.tellp()) == 0)
            {
                outfile << std::setw(15) << "Day" << "," << std::setw(15) << "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
                for (const DataTypes::Numeric& num : std::get<0>(queueVal)->data)
                {
                    outfile << std::setw(15) << num.mnem << ",";
                }
                outfile << "\n";
            }

            outfile << std::setw(15) << std::get<0>(queueVal)->day << "," << std::setw(15) << std::get<0>(queueVal)->millis << "," << std::setw(15) << std::get<0>(queueVal)->micros << "," << std::setw(15) << std::get<0>(queueVal)->sequenceCount << ",";
            for (const DataTypes::Numeric& num : std::get<0>(queueVal)->data)
            {
                switch (num.tag)
                {
                case DataTypes::Numeric::I32: outfile << std::setw(15) << std::right << num.i32; break;
                case DataTypes::Numeric::U32: outfile << std::setw(15) << std::right << num.u32; break;
                case DataTypes::Numeric::F64: outfile << std::setw(15) << std::right << num.f64; break;
                default: break;
                }
                outfile << ",";
            }
            outfile << "\n";
            std::get<1>(queueVal)->unlock();
        }
        else
            return;
    }
}
void ThreadPoolServer::start()
{
    for (uint32_t i = 0; i < m_num_threads; ++i)
    {
        m_threads.emplace_back(std::thread(&ThreadPoolServer::ThreadMain, this, std::ref(m_queue), std::ref(m_instrument), std::ref(m_outfiles)));
    }
}

void ThreadPoolServer::exec(std::unique_ptr<DataTypes::Packet> pack)
{
    m_queue.push(std::move(pack));
}

void ThreadPoolServer::join()
{
    std::cout << std::endl << "Waiting for writer threads to finish...";
    for (auto& thread : m_threads)
        thread.join();
    for (auto& stream : m_outfiles)
        stream.second.close();
}
