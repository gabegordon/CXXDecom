#include <fstream>
#include <iomanip>
#include <unordered_map>
#include "ThreadPoolServer.h"

void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue<DataTypes::Packet>& queue, const std::string instrument)
{
	std::unordered_map<uint32_t, std::ofstream> m_outfiles;
	while (true)
	{
		DataTypes::Packet pack;
		if (queue.listen(pack) == 0) //If we get a packet from the queue
		{
			if (pack.ignored)
				return;
			std::ofstream outfile;
			if (m_outfiles.count(pack.apid) == 0)
			{
				outfile.open("output/" + m_instrument + "_" + std::to_string(pack.apid) + ".txt", std::ios::ate);
				outfile << std::setw(15) << "Day" << "," << std::setw(15) << "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
				for (const DataTypes::Numeric& num : pack.data)
				{
					outfile << std::setw(15) << num.mnem << ",";
				}
			}
			else
				outfile = std::move(m_outfiles.at(pack.apid));

			outfile << "\n";
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
			m_outfiles[pack.apid] = std::move(outfile);
		}
	}
}
void ThreadPoolServer::start()
{
	for (size_t i = 0; i < m_num_threads; ++i)
	{
		m_threads.push_back(std::thread(&ThreadPoolServer::ThreadMain, this, std::ref(m_queue), std::ref(m_instrument)));
		m_threads.at(i).detach();
	}
}

void ThreadPoolServer::exec(const DataTypes::Packet pack)
{
	m_queue.push(pack);
}

