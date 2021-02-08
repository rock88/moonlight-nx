require 'fileutils'

class String
  def parse_addresses
    scan(/([\w\[\]\d]+?):[\w \d]+?\(moonlight \+ ([\w\d]+?)\)/)
  end

  def run_addr2line
    `/opt/devkitpro/devkitA64/bin/aarch64-none-elf-addr2line -e #{__dir__}/moonlight.elf -s -f -p -C -a #{self}`
  end
end

class Array
  def map_addresses
    map { |item| { 'name' => item[0], 'address' => item[1] } }
  end
end

class HBLoaderReportParser
  attr_accessor :exception_type, :thread_id, :registers, :stack_trace

  def initialize(report)
    @exception_type = report.scan(/Type:\s+([\w ]+)/)[0][0]
    @thread_id = report.scan(/Thread ID:\s+([\w ]+)/)[0][0]
    @registers = report.split('Stack Trace')[0].parse_addresses.map_addresses
    @stack_trace = report.split('Stack Trace')[1].parse_addresses.map_addresses
  end
end

class ThreadReportParser
  attr_accessor :thread_name, :thread_id, :registers, :stack_trace

  def initialize(report)
    @thread_name = 'Threads[' + report.scan(/^\[(\d+)\]/)[0][0] + ']'
    @thread_id = report.scan(/Thread ID:\s+([\w ]+)/)[0][0]
    @registers = report.split('Stack Trace')[0].parse_addresses.map_addresses
    @stack_trace = report.split('Stack Trace')[1].parse_addresses.map_addresses
  end
end

class CrashReportParser
  def initialize(path)
    reports = File.open(path, :encoding => 'utf-8').read.split(/^Process Info:|^Module Info:|^Threads/)
    @hbloader = HBLoaderReportParser.new(reports[1])
    @threads = reports.drop(3).map { |report| ThreadReportParser.new(report) }
  end

  def info(skip_threads)
    puts "HBLoader Exception Info:"
    puts "\tException Type: #{@hbloader.exception_type}"
    puts "\tCrashed Thread: #{@hbloader.thread_id}"
    puts ""
    puts "Registers:"
    @hbloader.registers.each { |register|
      puts "\t#{register['name']}: #{register['address']}"
      puts "\t#{register['address'].run_addr2line}"
      puts ""
    }
    puts "Stack Trace:"
    @hbloader.stack_trace.each { |register|
      puts "\t#{register['address'].run_addr2line}"
    }
    puts ""

    if skip_threads
      return
    end

    puts "Threads Info:"
    @threads.each { |thread|
      puts "#{thread.thread_name}:"
      puts "Registers:"
      thread.registers.each { |register|
        puts "\t#{register['name']}: #{register['address']}"
        puts "\t#{register['address'].run_addr2line}"
        puts ""
      }
      puts "Stack Trace:"
      thread.stack_trace.each { |register|
        puts "\t#{register['address'].run_addr2line}"
      }
      puts ""
    }
  end
end

def print_usage
  puts "Usage:"
  puts "\truby dbg.ruby options path.log"
  puts "\toptions: -s for skip threads info"
  puts "\tpath.log: full log path or just log file name, if it in crash_reports dir"
  exit -1
end

if ARGV.include?('--help') || ARGV.include?('-h')
  print_usage
end

path = ARGV.last

if path == nil
  print_usage
elsif !File.exist?(path)
  path = "#{__dir__}/crash_reports/#{path}"

  if !File.exist?(path)
    print_usage
  end
end

parser = CrashReportParser.new(path)
parser.info(ARGV.include?('-s'))
