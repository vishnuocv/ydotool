/*
    This file is part of ydotool.
	Copyright (C) 2019 Harry Austen
    Copyright (C) 2018-2019 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

// Local includes
#include "ydotool.hpp"
// C system includes
#include <assert.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <getopt.h>
// C++ system includes
#include <thread>
#include <functional>
// External libs
#include <boost/crc.hpp>
#include <evdevPlus/evdevPlus.hpp>

struct file_header {
    char magic[4];
    uint32_t crc32;
    uint64_t size;
    uint64_t feature_mask;
} __attribute__((__packed__));

struct data_chunk {
    uint64_t delay[2];
    uint16_t ev_type;
    uint16_t ev_code;
    int32_t ev_value;
} __attribute__((__packed__));

int fd_file = -1;

std::vector<uint8_t> record_buffer;
file_header header;

void dir_foreach(const std::string & path, const std::function<int(const std::string &, struct dirent *)> & callback) {
	DIR * dir;
	struct dirent * ent;

	if ((dir = opendir(path.c_str()))) {
		int dotdot_flag = 0;

		while ((ent = readdir(dir))) {

			if (dotdot_flag != 2) {
				if (strcmp(ent->d_name, ".") == 0) {
					dotdot_flag++;
					continue;
				}
				if (strcmp(ent->d_name, "..") == 0) {
					dotdot_flag++;
					continue;
				}
			}

			int rc = callback(path, ent);

			if (rc == 1)
				continue;
			else if (rc == 2)
				break;
		}

		closedir(dir);
	} else {
		throw std::system_error(errno, std::system_category(), strerror(errno));
	}
}

uint32_t crc32(const void *buf, size_t len) {
	boost::crc_32_type result;
	result.process_bytes(buf, len);
	return result.checksum();
}

void do_record(const std::vector<std::string> &__devices) {

	int fd_epoll = epoll_create(42);
	assert(fd_epoll > 0);

	for (auto &it : __devices) {
		std::cerr << "Using device: " << it << "\n";

		auto evDev = new evdevPlus::EventDevice(it);

		epoll_event eev {};
		eev.data.ptr = evDev;
		eev.events = EPOLLIN;

		assert(!epoll_ctl(fd_epoll, EPOLL_CTL_ADD, evDev->FD, &eev));

	}

	timespec tm_now, tm_now2, tm_diff;
	clock_gettime(CLOCK_MONOTONIC, &tm_now);

	epoll_event events[16];

	std::cerr << "Started recording\n";

	while (int rc = epoll_wait(fd_epoll, events, 16, -1)) {

		assert(rc > 0);

		for (int i=0; i<rc; i++) {
			auto & it = events[i];
			auto eev = (evdevPlus::EventDevice *)it.data.ptr;
			auto buf = eev->Read();

			data_chunk dat;

			clock_gettime(CLOCK_MONOTONIC, &tm_now2);

            // Calculate the difference
            if ((tm_now2.tv_nsec - tm_now.tv_nsec) < 0) {
                tm_diff.tv_sec = tm_now2.tv_sec - tm_now.tv_sec - 1;
                tm_diff.tv_nsec = tm_now2.tv_nsec - tm_now.tv_nsec + 1000000000;
            } else {
                tm_diff.tv_sec = tm_now2.tv_sec - tm_now.tv_sec;
                tm_diff.tv_nsec = tm_now2.tv_nsec - tm_now.tv_nsec;
            }

			tm_now = tm_now2;

			dat.delay[0] = tm_diff.tv_sec;
			dat.delay[1] = tm_diff.tv_nsec;
			dat.ev_type = buf.Type;
			dat.ev_code = buf.Code;
			dat.ev_value = buf.Value;

			record_buffer.insert(record_buffer.end(), (uint8_t *)&dat, (uint8_t *)&dat+sizeof(data_chunk));
		}
	}
}

std::vector<std::string> find_all_devices() {
	std::vector<std::string> ret;

	try {
		dir_foreach("/dev/input", [&](const std::string & path_base, struct dirent * ent) {
			if (ent->d_type != DT_CHR || ent->d_name[0] != 'e')
				return 1;

			std::string fullpath = path_base + "/" + std::string(ent->d_name);
			ret.push_back(std::move(fullpath));

			return 0;
		});
	} catch (...) {

	}

	return ret;
}

void recorder_help(){
	std::cerr << "Usage: recorder [--delay <ms] [--duration <ms>] [--record <output file> [devices]] [--replay <input file>]\n"
		  << "  --help                Show this help.\n"
		  << "  --record                \n"
		  << "  devices               Devices to record from. Default is all, including non-keyboard devices.\n"
		  << "  --replay                \n"
		  << "  --display               \n"
		  << "  --delay ms            Delay time before start recording/replaying. Default 5000ms.\n"
		  << "  --duration ms         Record duration. Otherwise use SIGINT to stop recording.\n"
		     "\n"
		     "The record file can't be replayed on an architecture with different endianness." << std::endl;
}

void generate_header() {
	auto & m = header.magic;
	m[0] = 'Y';
	m[1] = 'D';
	m[2] = 'T';
	m[3] = 'L';

	header.feature_mask = 0;
	header.size = record_buffer.size();
	header.crc32 = crc32(record_buffer.data(), record_buffer.size());
}

void stop_handler(__attribute__((unused)) int whatever) {
	std::cout << "Saving file...\n";
	generate_header();

	write(fd_file, &header, sizeof(header));
	write(fd_file, record_buffer.data(), record_buffer.size());

	close(fd_file);

	std::cout << "Done.\n";

	exit(0);
}

void do_replay() {
	struct stat statat;

	fstat(fd_file, &statat);

	if ((size_t)statat.st_size < sizeof(file_header)+sizeof(data_chunk)) {
		fprintf(stderr, "File too small\n");
		abort();
	}

	uint8_t * filedata = (uint8_t *)mmap(nullptr, statat.st_size, PROT_READ, MAP_SHARED, fd_file, 0);

	assert(filedata);

	file_header * file_hdr = (file_header *)filedata;
	auto file_end = filedata + statat.st_size;
	auto data_start = (filedata + sizeof(file_header));

	auto cur_pos = data_start;

	size_t size_cur = file_end - data_start;

	if (size_cur == file_hdr->size) {
		fprintf(stderr, "Size match\n");
	} else {
		fprintf(stderr, "Size mismatch: %zu != %zu\n", size_cur, file_hdr->size);
		abort();
	}

	auto crc32_cur = crc32(data_start, size_cur);

	if (crc32_cur == file_hdr->crc32) {
		fprintf(stderr, "CRC32 match\n");
	} else {
		fprintf(stderr, "CRC32 mismatch: %08x != %08x\n", crc32_cur, file_hdr->crc32);
		abort();
	}

	std::cerr << "Started replaying\n";

    const uInputPlus::uInput * uInputContext = ydotool_get_context();

	while (cur_pos < file_end) {
		auto dat = (data_chunk *)cur_pos;
		usleep(dat->delay[0] * 1000000 + dat->delay[1] / 1000);

		uInputContext->Emit(dat->ev_type, dat->ev_code, dat->ev_value);

		cur_pos += sizeof(data_chunk);
	}
}

void do_display() {
	struct stat statat;

	fstat(fd_file, &statat);

	if ((size_t)statat.st_size < sizeof(file_header)+sizeof(data_chunk)) {
		fprintf(stderr, "File too small\n");
		abort();
	}

	auto filedata = (uint8_t *)mmap(nullptr, statat.st_size, PROT_READ, MAP_SHARED, fd_file, 0);

	assert(filedata);

	auto file_hdr = (file_header *)filedata;
	auto file_end = filedata + statat.st_size;
	auto data_start = (filedata + sizeof(file_header));

	auto cur_pos = data_start;

	auto size_cur = file_end - data_start;
	auto crc32_cur = crc32(data_start, size_cur);


	printf("CRC32: 0x%08x / 0x%08x\n", crc32_cur, file_hdr->crc32);
	printf("Data length: %zu / %zu (%zu events)\n", file_hdr->size, size_cur, file_hdr->size / sizeof(data_chunk));
	puts("============================================");


	while (cur_pos < file_end) {
		auto dat = (data_chunk *)cur_pos;
		printf("Offset: 0x%lx\n", (uint8_t *)(dat)-filedata);
		printf("Delay: %" PRIu64 ".%09" PRIu64 " second\n", dat->delay[0], dat->delay[1]);
		printf("Event: %u, %u, %u\n", dat->ev_type, dat->ev_code, dat->ev_value);
		puts("-");
		cur_pos += sizeof(data_chunk);
	}
}

int recorder_run(int argc, char ** argv) {
	int delay = 5000;
	int duration = 0;
	int mode = 0;
    int opt = 0;

    typedef enum {
        opt_help,
        opt_record,
        opt_replay,
        opt_display,
        opt_delay,
        opt_duration
    } optlist_t;

    static struct option long_options[] = {
        { "help",     no_argument,       NULL, opt_help     },
        { "record",   no_argument,       NULL, opt_record   },
        { "replay",   no_argument,       NULL, opt_replay   },
        { "display",  no_argument,       NULL, opt_display  },
        { "delay",    required_argument, NULL, opt_delay    },
        { "duration", required_argument, NULL, opt_duration }
    };

    while ((opt = getopt_long_only(argc, argv, "hrpyd:u:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'r':
            case opt_record:
                mode = 1;
                break;
            case 'p':
            case opt_replay:
                mode = 2;
                break;
            case 'y':
            case opt_display:
                mode = 3;
                break;
            case 'd':
            case opt_delay:
                delay = strtoul(optarg, NULL, 10);
                break;
            case 'u':
            case opt_duration:
                duration = strtoul(optarg, NULL, 10);
                break;
            case 'h':
            case opt_help:
            case '?':
                recorder_help();
                return -1;
        }
    }

    if (!mode) {
        std::cerr << "Mode not specified!" << std::endl;
        recorder_help();
        return -1;
    }

    int extra_args = argc - optind;
    if (!extra_args) {
        std::cerr << "file not specified" << std::endl;
        recorder_help();
        return -1;
    }

	const std::string & filepath = argv[optind++];

	if (mode == 1)
		fd_file = open(filepath.c_str(), O_WRONLY|O_CREAT, 0644);
	else
		fd_file = open(filepath.c_str(), O_RDONLY);

	if (fd_file == -1) {
		std::cerr << "ydotool: " << argv[0] << ": error: failed to open " << filepath << ": "
			  << strerror(errno) << std::endl;
		return 2;
	}

	std::cerr << "Delay was set to "
		  << delay << " milliseconds.\n";

	if (mode == 1) {
        std::vector<std::string> device_list = {};
        if (extra_args > 1) {
            for (; optind != argc; optind++) {
                device_list.push_back( argv[optind] );
            }
        } else {
			device_list = find_all_devices();
			if (device_list.empty()) {
				std::cerr << "No event device found in /dev/input/" << std::endl;
				return -1;
			}
		}

		signal(SIGINT, stop_handler);

		if (duration)
			std::thread([duration]() {
				std::cerr << "Duration was set to "
					  << duration << " milliseconds.\n";
				usleep(duration * 1000);
				kill(getpid(), SIGINT);
			}).detach();

		if (delay)
			usleep(delay * 1000);

		do_record(device_list);
	} else if (mode == 2) {
		if (delay)
			usleep(delay * 1000);

		do_replay();
	} else if (mode == 3) {
		do_display();
	}

	return 0;
}
